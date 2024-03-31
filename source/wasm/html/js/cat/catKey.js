/**
 * キー入力
 * @author 猫大名 ねこ猫
 */
export default class {
    // CatKey

    /**
     * キー変換関数
     * 
     * KeyEventからキーコードを生成する
     * @type {function}
     */
    #keyCodeConverter;

    /**
     * キーに入力バッファ
     * 
     * 入力されたキーが一旦蓄えられる
     * dequeueKeyBuffer()で取得する。
     * @type {number|string[]}
     */
    #keyBuffer;

    /**
     * キーバッファの最大サイズ
     * @type {number}
     */
    #keyBufferSize;

    /**
     * キーの押下状態
     * 
     * 押されていればtrue。
     * @type {Map}
     */
    #keyDown;
    /**
     * @type {number}
     */
    #index = 1;

    /**
     * キーイベントの処理を他にやらせないようにする場合trueを設定する
     * @type {boolean}
     */
    #stopPropagation;

    #ctrlKey;
    #shiftKey;
    #capsLock;
    #altKey;

    /**
     * コンストラクタ
     * @param {*} document ドキュメント
     * @param {function} keyCodeConverter (keyイベント)=>{return UTF-32 or 文字列}
     */
    constructor(document, keyCodeConverter)
    {
        this.#keyDown = new Map();
        this.#keyCodeConverter = keyCodeConverter ? keyCodeConverter : this.#defaultKeyConverter;
        this.#keyBuffer = [];
        this.#keyBufferSize = 16;
        this.#shiftKey = false;
        this.#ctrlKey = false;
        this.#capsLock = false;
        document.addEventListener('keydown', (e)=>this.#keyDownHandler(this, e), false);
        document.addEventListener('keyup', (e)=>this.#keyUpHandler(this, e), false);
        this.#stopPropagation = true;
    }

    /**
     * キーイベント処理を他に処理させないようにするかどうか
     * @param {boolean} flag 他に処理させない場合はtrue
     */
    setStopPropagation(flag)
    {
        this.#stopPropagation = flag;
    }

    /**
     * デフォルトのキー変換関数
     * 
     * 数値の場合はUTF-32。文字列の場合は、制御コード。
     * 制御コードとは、カーソルキー、Home、Endなど画面を操作するもので、Unicodeで存在しないもの。
     * @param {KeyEvent} e キーイベント
     * @returns {number|string} 変換したキーコード
     */
    #defaultKeyConverter(e)
    {
        let keyCode = 0;
        const index = ' !"#$%&\'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~'.indexOf(e.key);
        if(index >= 0) {
            // Unicode
            if(e.ctrlKey) {
                // コントロールキーが押下されているときの処理
                if(index >= 0x20 && index < 0x40) {  // " "～？
                    keyCode = index - 0x20;
                } else if(index >= 0x41 && index < 0x5b) { // a～z
                    keyCode = index - 0x40;
                } else {
                    keyCode = index + 0x20;
                }
            } else {
                keyCode = index + 0x20;
            }
        } else {
            switch (e.key) {
                case 'Backspace':  keyCode = 0x08; break; // BS
                case 'Tab':        keyCode = 0x09; break; // HT
                case 'Enter':      keyCode = 0x0D; break; // CR
                case 'Pause':
                case 'Escape':     keyCode = 0x1B; break; // ESC

                // 制御コード
                case 'Delete':
                case 'ArrowRight':	// カーソル移動
                case 'ArrowLeft':
                case 'ArrowUp':
                case 'ArrowDown':
                case 'Home':	// Homeキー 行頭へ移動
                case 'End':		// Endキー	行末へ移動
                case 'KanjiMode': // 漢字モード
                case 'F1': // F1
                case 'F2': // F2
                case 'F3': // F3
                case 'F4': // F4
                case 'F5': // F5
                case 'F6': // F6
                case 'F7': // F7
                case 'F8': // F8
                case 'F9': // F9
                case 'F10': // F10
                    keyCode = e.key;
                    break;
            }
        }
        return keyCode;
    }

    #updateCapsLockState(e)
    {
        if(e.getModifierState("CapsLock") === false) {
            this.#capsLock = false;
        }
        else
        {
            this.#capsLock = true;
        }
    }
    
    /**
     * キー押下時に呼び出される
     * @param {CatKey} self CatKey
     * @param {KeyEvent} e キーイベント
     */
    #keyDownHandler(self, e) {
        self.#shiftKey = !!e.shiftKey;
        self.#ctrlKey = !!e.ctrlKey;
        self.#altKey = !!e.altKey;
        self.#updateCapsLockState(e);

        // キー押下状態の更新
        {
            self.#keyDown.set(e.keyCode, { prio: self.#index, e: e });
            self.#index++;
            self.#keyDown = new Map([...this.#keyDown].sort((a,b)=> b[1].prio - a[1].prio).slice(0, 6));
            //console.log("#keyDownHandler keyCode:" + e.keyCode + " key:" + e.key + " cnt:" + self.#keyDown.size);
        }

        if(document.body !== document.activeElement) {
            return false; // フォーカスがなかったら処理しない
        }		

        // キー入力
        const keyCode = self.#keyCodeConverter(e);
        if(keyCode) {
            // バッファに積む
            self.enqueueKeyBuffer(keyCode);
        }

        if(self.#stopPropagation) {
            // イベントを処理したので、イベントの処理を他にやらせないようにする
            e.preventDefault();
            e.stopPropagation();
        }
    }

    /**
     * キーアップ時に呼び出される
     * @param {CatKey} self CatKey
     * @param {KeyEvent} e キーイベント
     */
    #keyUpHandler(self, e) {
        self.#updateCapsLockState(e);

        // キー押下状態の更新
        {
            if(self.#keyDown.has(e.keyCode)) { self.#keyDown.delete(e.keyCode); }
            self.#keyDown = new Map([...this.#keyDown].sort((a,b)=> b[1].prio - a[1].prio).slice(0, 6));
            //console.log("#keyUpHandler keyCode:" + e.keyCode + " key:" + e.key + " cnt:" + self.#keyDown.size);
        }

        if(document.body !== document.activeElement) {
            return false; // フォーカスがなかったら処理しない
        }		

        if(self.#stopPropagation) {
            // イベントを処理したので、イベントの処理を他にやらせないようにする
            e.preventDefault();
            e.stopPropagation();
        }
    }

    /**
     * キーバッファに入れる
     * @param {number|string} keyCode キーコード
     * @returns {boolean} キューに入れれたら true
     */
    enqueueKeyBuffer(keyCode) {
        // キューに積む
        if(this.#keyBuffer.length < this.#keyBufferSize) {
            this.#keyBuffer.unshift(keyCode);
            return true;
        } else {
            return false;
        }
    }

    /**
     * キーバッファから取得する
     * @returns {number|string} キーコード。キーバッファが空なら、0を返す
     */
    dequeueKeyBuffer() {
        if(this.#keyBuffer.length > 0) {
            return this.#keyBuffer.pop();
        } else {
            return 0;
        }
    }

    /**
     * キーバッファをクリアする
     */
    keyBufferClear() {
        this.#keyBuffer.length = 0;
        this.#keyDown.clear();
        this.#index = 1;

        this.#shiftKey = false;
        this.#ctrlKey = false;
        this.#capsLock = false;
    }

    /**
     * キーが押下されているかどうか
     * @param {number|string} keyCode 調べるキーコード
     * @returns {boolean} 押下されていたら true を返す
     */
    isKeyDown(keyCode)
    {
        for(let e of this.#keyDown) {
            if(this.#keyCodeConverter(e[1].e) == keyCode) {
                return true;
            }
        }
        return false;
    }
    isKeyDownRaw(keyCode)
    {
        for(let e of this.#keyDown) {
            if(e[1].e.key == keyCode) {
                return true;
            }
        }
        return false;
    }

    /**
     * 最後に入力されたキーが押下されているかどうか
     * @returns {number|string} キーコード。押下されていなかったら0を返す。
     */
    inKey() {
        // 最後付近に押下されたものをINKEYで返すように
        //
        // そうすると、方向キー押下中に、スペース押下みたいなので、
        // スペースを離したときに、方向キーの入力を返すことができるようになる。
        // (移動中に弾を撃ったりなどが、スムーズになるかも？)
        if(this.#keyDown.size > 0) {
            let temp = [...this.#keyDown].sort((a,b)=> b[1].prio - a[1].prio);
            return this.#keyCodeConverter(temp[0][1].e);
        }
        return 0;
    }

    getGameKey()
    {
        return (this.#ctrlKey ? 0x01 : 0)
            | (this.#shiftKey ? 0x02 : 0);
    }
}