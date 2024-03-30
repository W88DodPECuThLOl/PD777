export default class {
    // CatFullScreen
    #document;
    #fullScreenTargetElementId;

    /**
     * コンストラクタ
     * 
     * @param {*} document 
     * @param {string} fullScreenTargetElementId フルスクリーンにする要素のID
     * @param {string} fullScreenTriggerElementId フルスクリーンに切り替えるとき要素のID
     * @param {function} onEnterFullscreen フルスクリーンになったときの処理
     * @param {function} onExitFullscreen フルスクリーンから戻ったときの処理
     */
    constructor(document, fullScreenTargetElementId, fullScreenTriggerElementId, onEnterFullscreen, onExitFullscreen)
    {
        this.#document = document;
        this.#fullScreenTargetElementId = fullScreenTargetElementId;
        // 切り替わりのイベントを登録
        this.#document.documentElement.addEventListener("fullscreenchange", 
            evt => {
                if (!this.#document.fullscreenElement) {
                    onExitFullscreen();
                } else if (this.#document.exitFullscreen) {
                    onEnterFullscreen(this.#document.fullscreenElement);
                }
            }
        );
        // フルスクリーンの切り替えトリガー登録
        if(fullScreenTriggerElementId) {
            const fullScreenTriggerElement = document.getElementById(fullScreenTriggerElementId);
            if(fullScreenTriggerElement) {
                fullScreenTriggerElement.addEventListener("click", evt => { this.toggle(); });
            }
        }
    }
    /**
     * 全画面と普通の画面を切りかえる
     */
    toggle()
    {
        if (!document.fullscreenElement) {
            const elem = this.#document.getElementById(this.#fullScreenTargetElementId);
            elem.requestFullscreen();
        } else if (this.#document.exitFullscreen) {
            this.#document.exitFullscreen();
        }
    }
}
