"use strict";

import CatKey from './cat/catKey.js';
import CatGamePad from './cat/catGamePad.js';

export class uPD777 {
    /**
     * Wasmのエクスポート部分
     */
    wasm;

    /**
     * 使用するヒープサイズ(64KiBの倍数であること)
     */
    #heapSize = 16*1024*1024; // 16MiB

    /**
     * Wasmのメモリ
     */
    #memory;

    /**
     * ゲームパッド入力
     */
    #gamePad;

    /**
     * キーボード入力
     */
    //#keyboard;

    /**
     * 描画するキャンパス
     */
    #canvasCtx;

    #previousRenderTimestamp;
    #previousTimestamp;

    #audio;

    /**
     * @param {string} codeFilename コードファイル
     */
    readAndSetupCode(codeFilename) {
        const fileReader = new XMLHttpRequest() ;
        fileReader.open("get", codeFilename, true) ;
        fileReader.responseType = "arraybuffer" ;

        fileReader.onerror = () => {
            alert("読み込み中にエラーが発生しました:" + codeFilename + "\nstatus:" + fileReader.status);
        };
        fileReader.onload = (evnt) => {
            if(fileReader.status >= 400) {
                alert("読み込み中にエラーが発生しました:" + codeFilename + "\nstatus:" + fileReader.status);
                return;
            }
            const objArray = fileReader.response;
            const objFile  = new Uint8Array(objArray);
            this.setupCode(objFile);
        }
        fileReader.send();
    }

    /**
     * @param {string} patternFilename パターンファイル名
     */
    readAndSetupPattern(patternFilename) {
        const fileReader = new XMLHttpRequest() ;
        fileReader.open("get", patternFilename, true) ;
        fileReader.responseType = "arraybuffer" ;

        fileReader.onerror = () => {
            alert("読み込み中にエラーが発生しました:" + patternFilename + "\nstatus:" + fileReader.status);
        };
        fileReader.onload = (evnt) => {
            if(fileReader.status >= 400) {
                alert("読み込み中にエラーが発生しました:" + patternFilename + "\nstatus:" + fileReader.status);
                return;
            }
            const objArray = fileReader.response;
            const objFile  = new Uint8Array(objArray);
            this.setupPattern(objFile);
        }
        fileReader.send();
    }

    setupCode(data) {
        // WASM側でメモリ確保して、そこへ書き込む
        const fileMemoryHandle = this.wasm.memoryAllocate(data.byteLength);
        const fileMemory = new Uint8Array(this.#memory.buffer, fileMemoryHandle, data.byteLength);
        for(let i = 0; i < data.byteLength; ++i) {
            fileMemory[i] = data[i];
        }
        this.wasm.setupCode(fileMemoryHandle, data.byteLength);
        this.wasm.memoryFree(fileMemoryHandle); // 忘れずにメモリを解放しておく
        this.reset();
    }

    setupPattern(data) {
        // WASM側でメモリ確保して、そこへ書き込む
        const fileMemoryHandle = this.wasm.memoryAllocate(data.byteLength);
        const fileMemory = new Uint8Array(this.#memory.buffer, fileMemoryHandle, data.byteLength);
        for(let i = 0; i < data.byteLength; ++i) {
            fileMemory[i] = data[i];
        }
        this.wasm.setupPattern(fileMemoryHandle, data.byteLength);
        this.wasm.memoryFree(fileMemoryHandle); // 忘れずにメモリを解放しておく
    }

    setupAuto(data) {
        // WASM側でメモリ確保して、そこへ書き込む
        const fileMemoryHandle = this.wasm.memoryAllocate(data.byteLength);
        const fileMemory = new Uint8Array(this.#memory.buffer, fileMemoryHandle, data.byteLength);
        for(let i = 0; i < data.byteLength; ++i) {
            fileMemory[i] = data[i];
        }
        this.wasm.setupAuto(fileMemoryHandle, data.byteLength);
        this.wasm.memoryFree(fileMemoryHandle); // 忘れずにメモリを解放しておく
    }

    /**
     * 読み込みと初期化
     * @param{string}	wasmFilename	読み込むWASMファイル
     * @returns 
     */
    start(wasmFilename, audio, queryString)
    {
        this.#audio = audio;
        const importObject = {
            // メモリ
            env: { memory: this.#memory },
            // サウンド
            sound: {
                writeFLS: (clockCounter, value, reverberatedSoundEffect)=>{ audio.writeFLS(clockCounter, value, reverberatedSoundEffect); },
                writeFRS: (clockCounter, value, reverberatedSoundEffect)=>{ audio.writeFRS(clockCounter, value, reverberatedSoundEffect); },
            }
        };

        return WebAssembly.instantiateStreaming(fetch(wasmFilename), importObject).then(
            (obj) => {
                // WASM側から提供されている関数や変数など
                this.wasm = obj.instance.exports;
                // 初期化
                this.wasm.setupHeap(this.wasm.__heap_base, this.#heapSize - this.wasm.__heap_base);
                this.wasm.initialize();
                // コードの読み込み
                if(queryString) {
                    const urlParams = new URLSearchParams(queryString);
                    const urlCode = urlParams.get("code");
                    const urlPtn  = urlParams.get("ptn");
                    if(urlPtn) {
                        this.readAndSetupPattern(urlPtn);
                    }
                    if(urlCode) {
                        this.readAndSetupCode(urlCode);
                    }
                }
                return this;
            }
        ).then(
            (cpu)=>{ cpu.update(0); }
        );
    }

    /**
     * コンストラクタ
     */
    constructor(navigator, document, canvasCtx)
    {
        // WASM用のメモリ確保
        this.#memory = new WebAssembly.Memory({ initial: ~~(this.#heapSize/(64*1024)), maximum: ~~(this.#heapSize/(64*1024) + 1) });
        // 入力関連
        this.#gamePad = new CatGamePad(navigator);
        //this.#keyboard = new CatKey(document);
        // グラフィック関連
        canvasCtx.setAttribute("width", 375);
        canvasCtx.setAttribute("height", 240);
        this.#canvasCtx = canvasCtx.getContext("2d");
    }

    /**
     * リセットする
     */
    reset() {
        this.wasm.clearKeyStatus();
        this.wasm.reset();
        this.#audio.reset();
    }

    /**
     * 入力の更新処理
     */
    #inputUpdate() {
        // 入力を更新
        this.wasm.clearKeyStatus();
        if(this.#gamePad) {
            // ゲームパッドからの入力
            this.#gamePad.update();

            // 入力を反映
            /**
             * K1  K2  K3  K4  K5  K6  K7
             * ----------------------------------------
             * STA L1L L1R SEL AUX 6   7    | [A08]
             * 1   L2L L2R 4   5   6   7    | [A09]
             * 1   2   3   4   5   P4  P3   | [A10]
             * 1   2   3   4   5   P2  P1   | [A11]
             * LL  L   C   R   RR  6   7    | [A12]
             */
            const keyGameStart   = (this.#gamePad.buttons[this.#gamePad.BUTTON_START_INDEX].current || this.#gamePad.buttons[this.#gamePad.BUTTON_R3_INDEX].current) ? 0x01 : 0x00;
            const keyLever1Left  = this.#gamePad.buttons[this.#gamePad.BUTTON_VLEFT_INDEX].current ? 0x02 : 0x00;
            const keyLever1Right = this.#gamePad.buttons[this.#gamePad.BUTTON_VRIGHT_INDEX].current ? 0x04 : 0x00;
            const keyGameSelect  = (this.#gamePad.buttons[this.#gamePad.BUTTON_BACK_INDEX].current || this.#gamePad.buttons[this.#gamePad.BUTTON_L3_INDEX].current) ? 0x08 : 0x00;
            const keyLever2Left  = this.#gamePad.buttons[this.#gamePad.BUTTON_LB_INDEX].current ? 0x02 : 0x00;
            const keyLever2Right = this.#gamePad.buttons[this.#gamePad.BUTTON_RB_INDEX].current ? 0x04 : 0x00;
            const keyPush4       = this.#gamePad.buttons[this.#gamePad.BUTTON_A_INDEX].current ? 0x20 : 0x00;
            const keyPush3       = this.#gamePad.buttons[this.#gamePad.BUTTON_Y_INDEX].current ? 0x40 : 0x00;
            const keyPush2       = this.#gamePad.buttons[this.#gamePad.BUTTON_B_INDEX].current ? 0x20 : 0x00;
            const keyPush1       = this.#gamePad.buttons[this.#gamePad.BUTTON_X_INDEX].current ? 0x40 : 0x00;
            const keyUp          = this.#gamePad.buttons[this.#gamePad.BUTTON_VUP_INDEX].current ? 0x40 : 0x00;
            const keyDown        = this.#gamePad.buttons[this.#gamePad.BUTTON_VDOWN_INDEX].current ? 0x20 : 0x00;
            const keyDebug       = false ? 0x80 : 0x00;
            //
            const A08Value = keyGameStart | keyLever1Left | keyLever1Right | keyGameSelect;
            const A09Value = keyLever2Left | keyLever2Right;
            const A10Value = keyPush4 | keyPush3;
            const A11Value = keyPush2 | keyPush1;
            const A12Value = 0;
            const SpecialValue = keyUp | keyDown | keyDebug;
            this.wasm.setKeyStatus(0, A08Value); // A08
            this.wasm.setKeyStatus(1, A09Value); // A09
            this.wasm.setKeyStatus(2, A10Value); // A10
            this.wasm.setKeyStatus(3, A11Value); // A11
            this.wasm.setKeyStatus(4, A12Value); // A12
            this.wasm.setKeyStatus(5, SpecialValue); // Special
        }
    }

    /**
     * CPU更新処理
     * @param {number} clock 実行するクロック数
     */
    #cpuUpdate(clock) {
        // CPUを更新
        this.wasm.execute(clock | 0);
    }

    /**
     * 更新処理
     * @param {number} timestamp ミリ秒単位
     */
    update(timestamp)
    {
        if (this.#previousRenderTimestamp === undefined) {
            this.#previousRenderTimestamp = timestamp;
            this.#previousTimestamp = timestamp;
        }
        // 前回のフレームからの経過時間(ms)
        const elapsed = timestamp - this.#previousTimestamp;
        this.#previousTimestamp = timestamp;
        // 前回の描画してからの経過時間(ms)
        const renderElapsed = timestamp - this.#previousRenderTimestamp;

        if(elapsed < 3*1000) { // 余りにも長い時間更新が途絶えていたら無視する
            // 入力の更新処理
            this.#inputUpdate();

            // CPU更新
            // CLOCK input  3.579545 MHz
            // CPU Clock    1.431818 MHz (= 3.579545 MHz / 2.5)
            this.#cpuUpdate(elapsed * 1431.818);

            // 画面描画
            if(renderElapsed >= 1000/60) {
                this.#previousRenderTimestamp += 1000/60;
                // 画像取得して描画
                this.#getVRAMImage(this.#canvasCtx);
            }
        } else {
            this.#previousRenderTimestamp = timestamp;
            this.#previousTimestamp = timestamp;
        }

        // VSync待ち
        requestAnimationFrame((timestamp)=>this.update(timestamp));
    }

    /**
     * VRAMをcanvasに描画
     * 
     * canvasのイメージデータはrgbaの順番で、各0x00～0xFFの値
     * @param {*} canvasCtx 
     */
    #getVRAMImage(canvasCtx) {
        if(this.wasm.isVRAMDirty()) {
            // 変換されたイメージを取得して
            const imagePtr = this.wasm.getVRAMImage(); // メモ）内部で、VRAMDirtyフラグリセットしている
            const src = new Uint8Array(this.#memory.buffer, imagePtr, 375*240*4);
            // キャンバスのイメージデータを作成し
            const dstImageData = canvasCtx.createImageData(375, 240);
            // コピー
            dstImageData.data.set(src);
            // 描画
            canvasCtx.putImageData(dstImageData, 0, 0);
        }
    }
}
