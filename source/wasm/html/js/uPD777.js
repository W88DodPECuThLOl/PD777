"use strict";

class uPD777 {
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
	 * WASMのセットアップ
	 * 
	 * 読み込みと初期化
	 * @param{string}	wasmFilename	読み込むWASMファイル
	 * @returns 
	 */
	setup(wasmFilename)
	{
		const importObject = {
			// メモリ
			env: { memory: this.#memory }
		};

		return WebAssembly.instantiateStreaming(fetch(wasmFilename), importObject).then(
			(obj) => {
				// WASM側から提供されている関数や変数など
				this.wasm = obj.instance.exports;
				// 初期化
				this.wasm.setupHeap(this.wasm.__heap_base, this.#heapSize - this.wasm.__heap_base);
				this.wasm.initialize();
				return this;
			}
		);
	}

	/**
	 * コンストラクタ
	 */
	constructor()
	{
		// WASM用のメモリ確保
		this.#memory = new WebAssembly.Memory({ initial: ~~(this.#heapSize/(64*1024)), maximum: ~~(this.#heapSize/(64*1024) + 1) });
	}

	/**
	 * リセットする
	 */
	reset() {
	}

	/**
	 * 更新処理
	 * @param {number} clock 実行するクロック数
	 */
	update(clock) {
		this.wasm.execute(clock | 0);
	}

	/**
	 * VRAMをcanvasに描画
	 * 
	 * canvasのイメージデータはrgbaの順番で、各0x00～0xFFの値
	 * @param {*} canvasCtx 
	 */
	getVRAMImage(canvasCtx) {
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
