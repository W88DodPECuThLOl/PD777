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
     * @type {CatGamePad}
     */
    #gamePad;

    /**
     * キーボード入力
     * @type {CatKey}
     */
    #keyboard;

    /**
     * 描画するキャンパス
     */
    #canvasCtx;

    #previousRenderTimestamp;
    #previousTimestamp;

    /**
     * オーディオ
     */
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
        this.#keyboard = new CatKey(document);
        this.#keyboard.setStopPropagation(false); // キー処理を他のでもできるように
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
        let keyGameStart   = false;
        let keyLever1Left  = false;
        let keyLever1Right = false;
        let keyGameSelect  = false;
        let keyLever2Left  = false;
        let keyLever2Right = false;
        let keyPush4       = false;
        let keyPush3       = false;
        let keyPush2       = false;
        let keyPush1       = false;
        let keyUp          = false;
        let keyDown        = false;
        let keyDebug       = false;

        if(this.#gamePad) {
            // ゲームパッドからの入力
            this.#gamePad.update();

            // LEVER SWITCH, START, SELECT
            if(this.#gamePad.buttons[this.#gamePad.BUTTON_START_INDEX].current || this.#gamePad.buttons[this.#gamePad.BUTTON_R3_INDEX].current) { keyGameStart = true; }
            if(this.#gamePad.buttons[this.#gamePad.BUTTON_VLEFT_INDEX].current) { keyLever1Left = true; }
            if(this.#gamePad.buttons[this.#gamePad.BUTTON_VRIGHT_INDEX].current) { keyLever1Right = true; }
            if(this.#gamePad.buttons[this.#gamePad.BUTTON_BACK_INDEX].current || this.#gamePad.buttons[this.#gamePad.BUTTON_L3_INDEX].current) { keyGameSelect = true; }
            if(this.#gamePad.buttons[this.#gamePad.BUTTON_LB_INDEX].current) { keyLever2Left = true; }
            if(this.#gamePad.buttons[this.#gamePad.BUTTON_RB_INDEX].current) { keyLever2Right = true; }
            // PUSH1～PUSH4
            if(this.#gamePad.buttons[this.#gamePad.BUTTON_A_INDEX].current) { keyPush4 = true; }
            if(this.#gamePad.buttons[this.#gamePad.BUTTON_Y_INDEX].current) { keyPush3 = true; }
            if(this.#gamePad.buttons[this.#gamePad.BUTTON_B_INDEX].current) { keyPush2 = true; }
            if(this.#gamePad.buttons[this.#gamePad.BUTTON_X_INDEX].current) { keyPush1 = true; }
            // etc.
            if(this.#gamePad.buttons[this.#gamePad.BUTTON_VUP_INDEX].current) { keyUp = true; }
            if(this.#gamePad.buttons[this.#gamePad.BUTTON_VDOWN_INDEX].current) { keyDown = true; }
            if(false) { keyDebug = true; }
        }
        // キーボードからの入力
        {
            // LEVER SWITCH, START, SELECT
            if(this.#keyboard.isKeyDown(0x71) || this.#keyboard.isKeyDown(0x51)) { keyGameSelect = true; } // 'Q'
            if(this.#keyboard.isKeyDown(0x61) || this.#keyboard.isKeyDown(0x41)) { keyLever1Left = true; } // 'A'
            if(this.#keyboard.isKeyDown(0x64) || this.#keyboard.isKeyDown(0x44)) { keyLever1Right = true; } // 'D'
            if(this.#keyboard.isKeyDown(0x65) || this.#keyboard.isKeyDown(0x45)) { keyGameStart = true; } // 'E'
            if(this.#keyboard.isKeyDown(0x1B)) { keyGameSelect = true; } // ESC or BREAK
            if(this.#keyboard.isKeyDown(0x0D)) { keyGameStart = true; } // ENTER
            // PUSH1～PUSH4
            if(this.#keyboard.isKeyDown('ArrowLeft')) { keyPush1 = true; }
            if(this.#keyboard.isKeyDown('ArrowRight')) { keyPush2 = true; }
            if(this.#keyboard.isKeyDown('ArrowUp')) { keyPush3 = true; }
            if(this.#keyboard.isKeyDown('ArrowDown')) { keyPush4 = true; }
            if(this.#keyboard.isKeyDown(0x68) || this.#keyboard.isKeyDown(0x48)) { keyPush1 = true; } // 'H'
            if(this.#keyboard.isKeyDown(0x6A) || this.#keyboard.isKeyDown(0x4A)) { keyPush2 = true; } // 'J'
            if(this.#keyboard.isKeyDown(0x6B) || this.#keyboard.isKeyDown(0x4B)) { keyPush3 = true; } // 'K'
            if(this.#keyboard.isKeyDown(0x6C) || this.#keyboard.isKeyDown(0x4C)) { keyPush4 = true; } // 'L'
            if(this.#keyboard.isKeyDown(0x20)) { keyPush1 = true; } // ' '
            // etc.
            if(this.#keyboard.isKeyDown(0x77) || this.#keyboard.isKeyDown(0x57)) { keyUp = true; } // 'W'
            if(this.#keyboard.isKeyDown(0x73) || this.#keyboard.isKeyDown(0x53)) { keyDown = true; } // 'S'
        }
        // 入力を反映
        {
            /**
             * K1  K2  K3  K4  K5  K6  K7
             * ----------------------------------------
             * STA L1L L1R SEL AUX 6   7    | [A08]
             * 1   L2L L2R 4   5   6   7    | [A09]
             * 1   2   3   4   5   P4  P3   | [A10]
             * 1   2   3   4   5   P2  P1   | [A11]
             * LL  L   C   R   RR  6   7    | [A12]
             */
            const A08Value = (keyGameStart ? 0x01 : 0x00)
                            | (keyLever1Left ? 0x02 : 0x00)
                            | (keyLever1Right ? 0x04 : 0x00)
                            | (keyGameSelect ? 0x08 : 0x00);
            const A09Value = (keyLever2Left ? 0x02 : 0x00)
                            | (keyLever2Right ? 0x04 : 0x00);
            const A10Value = (keyPush4 ? 0x20 : 0x00) | (keyPush3 ? 0x40 : 0x00);
            const A11Value = (keyPush2 ? 0x20 : 0x00) | (keyPush1 ? 0x40 : 0x00);
            const A12Value = 0;
            const SpecialValue = (keyUp ? 0x40 : 0x00)
                            | (keyDown ? 0x20 : 0x00)
                            | (keyDebug ? 0x80 : 0x00);
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
