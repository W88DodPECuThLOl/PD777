export class CatAudio {
    /**
     * オーディオコンテキスト
     * @type {AudioContext}
     */
    #audioCtx = null;
    #module = './js/cat/catAudioWorkletProcessor.js';
    #processorName = 'catAudioWorkletProcessor';
    gainWorkletNode;

    constructor()
    {
    }

    async init()
    {
        this.term();

        this.#audioCtx = new AudioContext();
		await this.#audioCtx.resume(); // 明示的にリジュームする

		//console.log("AudioContext.baseLatency:" + this.#audioCtx.baseLatency + " // AudioDestinationNodeから音声サブシステムに音声を渡す際に発生する処理遅延の秒数");
        //console.log("AudioContext.outputLatency:" + this.#audioCtx.outputLatency + " // 現在の音声コンテキストの出力レイテンシーの見積");
        //console.log("AudioContext.sampleRate:" + this.#audioCtx.sampleRate);
		
		await this.#audioCtx.audioWorklet.addModule(this.#module);
		this.gainWorkletNode = new AudioWorkletNode(this.#audioCtx, this.#processorName,  {
			outputChannelCount: [1]
		});
		const vol = new GainNode(this.#audioCtx, { gain: 1.0 });
		this.gainWorkletNode.connect(vol).connect(this.#audioCtx.destination);
    }

    term()
    {
        if(this.#audioCtx) {
            this.#audioCtx.close();
            this.#audioCtx = null;
        }
    }

    resume()
    {
        this.#audioCtx.resume().then(() => {
            console.log('Playback resumed successfully');
        });
    }

	/**
	 * Tickをリセットして、CPU側のクロックと同期させる
	 */
	reset()
	{
		if(this.#audioCtx && this.gainWorkletNode) {
			this.gainWorkletNode.port.postMessage({
				message: 'reset'
			});
		}
	}
	
	/**
	 * サウンドデバイスのレジスタに値を書き込む
	 * 
	 * executedClockは、同期用の値で、reset()が呼び出されてからどのくらいクロックが進んだのかを設定する。
	 * @param {number} executedClock 書き込まれた時の経過しているクロック
	 * @param {number} no サウンドデバイスの番号(0～)
	 * @param {number} reg 書き込むレジスタ番号
	 * @param {number} value 書き込む値
	 */
	writeFLS(clockCounter, value, reverberatedSoundEffect)
	{
		if(this.#audioCtx && this.gainWorkletNode) {
			this.gainWorkletNode.port.postMessage({
				message: 'writeRegister',
				channel: 0,
				value: value,
				reverberatedSoundEffect: reverberatedSoundEffect
			});
		}
	}

	writeFRS(clockCounter, value, reverberatedSoundEffect)
	{
		if(this.#audioCtx && this.gainWorkletNode) {
			this.gainWorkletNode.port.postMessage({
				message: 'writeRegister',
				channel: 1,
				value: value,
				reverberatedSoundEffect: reverberatedSoundEffect
			});
		}
	}

	/**
	 * ボリュームを設定する
	 * @param {number} value 書き込む値
	 */
	setVolume(value)
	{
		if(this.#audioCtx && this.gainWorkletNode) {
			this.gainWorkletNode.port.postMessage({
				message: 'setVolume',
				value: value
			});
		}
	}
}
