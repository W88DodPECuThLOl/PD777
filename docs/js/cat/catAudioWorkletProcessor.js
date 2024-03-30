class SourceInfo {
    /**
	 * @type {number}
     */
    phase;

    /**
	 * @type {number}
     */
    freq;

    // REV EFFECT
    /**
	 * @type {boolean}
     */
    rev;
    /**
	 * @type {number}
     */
    revVolume;
    /**
	 * @type {number}
     */
    revTime;

    constructor()
    {
        this.phase = 0.0;
        this.freq = 0.0;
        this.rev = false;
        this.revVolume = 0.0;
        this.revTime = 0;
    }
}

class GainProcessor extends AudioWorkletProcessor {
    /**
     * 有効かどうか
     * 
     * 初期化が終わって使えるようになると true になる
     * @type {boolean}
     */
    #enable = false;

    /**
     * 残響効果の開始時間(秒)
     * 
     * @type {number}
     */
    #REV_EFFECT_NOTE_OFF_TIME = 0.04;

    /**
     * 残響効果の減衰の係数（1に近い程、余韻が長くなる）
     * 
     * @type {number}
     */
    #REV_EFFECT_ATTENUATION = 0.9994;


    /**
     * @type {SourceInfo[]}
     */
    #sourceInfo;

    /**
     * マスターボリューム(0～)
     * 
     * @type {number}
     */
    #masterVolume;

	/**
	 * コンストラクタ
	 * @param {*} wasmBinary 
	 */
	constructor(wasmBinary) {
		super();

        this.#sourceInfo  = new Array(2);
		this.#sourceInfo[0] = new SourceInfo();
		this.#sourceInfo[1] = new SourceInfo();
		this.#masterVolume = 0.3;

		// メッセージ受け取りの登録
		this.port.onmessage = (event)=>{
			if(this.#enable) {
				const message = event.data;
				switch(message.message) {
					// 内部のチックをリセットして、CPU側と同期する
					case 'reset':
                        {
							this.#sourceInfo[0].freq = 0;
							this.#sourceInfo[1].freq = 0;
						}
						break;
					// レジスタ書き込み
					case 'writeRegister':
						{
							const channel                 = message.channel;
							const value                   = message.value;
							const reverberatedSoundEffect = message.reverberatedSoundEffect;
							this.#addScore(channel, value, reverberatedSoundEffect);
						}
						break;
					// 音量を設定する
					case 'setVolume':
						this.#masterVolume = message.value;
						break;
				}
			}
		};
		// 準備完了
		this.#enable = true;
	}

	/**
	 * デストラクタ
	 */
	destructor()
	{
		this.#enable = false;
	}

    #getSamplesPerSec()
    {
        return 48000.0; // @todo
    }

    #addScore(channel, value, reverberatedSoundEffect)
    {
		let src = this.#sourceInfo[channel];
		src.freq = this.#convert(value);

		if(src.rev != reverberatedSoundEffect) {
			src.rev = reverberatedSoundEffect;
			if(src.rev) {
				// REV on
				src.revTime = 0;
				src.revVolume = 1.0;
			}
		}
	}

	#convert(value) {
		/*
		A counter which initial value is FLS[7:1] or FRS[7:1] down-counts every horizontal period (15.734 KHz).
		Two sets of sound down-counters are implemented and the sounds generated are mixed together. Therefore,
		SOUND pin outputs monaural sound superimposed the two sound sources.
		*/
		if(value >= 2) {
			return 15734.0 / (value - 1);
		} else {
			return 0.0;
		}
	}

    process(inputs, outputs, parameters) {
		if(!this.#enable) {
			return true;
		}

		const len = outputs[0][0].length;
        for(let index = 0; index < len; ++index) {
            let value = 0.0;
			for(let ch = 0; ch < 2; ++ch) {
                let src = this.#sourceInfo[ch];
                if(src.freq > 0.0) {
                    let v = (src.phase > 3.141592653) ? 0.0 : 0.08;
                    const step = src.freq *(2.0*3.141592653) / this.#getSamplesPerSec();
                    src.phase += step;
					while(src.phase > 2.0 * 3.141592653) {
						src.phase -= 2.0 * 3.141592653;
					}
                    // REVの処理
                    if(src.rev) {
                        v *= src.revVolume;
                        if(src.revTime++ > this.#getSamplesPerSec() * this.#REV_EFFECT_NOTE_OFF_TIME) {
                            src.revVolume *= this.#REV_EFFECT_ATTENUATION;
                        }
                    }
                    // 合成
                    value += v;
                }
            }
			value *= this.#masterVolume;
            outputs[0][0][index] = value;
        }

        return true;
	}

	static get parameterDescriptors() {
		return [{
			name: "customGain",
			defaultValue: 1,
			minValue: 0,
			maxValue: 1,
			automationRate: "a-rate"
		}];
	}
}
registerProcessor('catAudioWorkletProcessor', GainProcessor);
