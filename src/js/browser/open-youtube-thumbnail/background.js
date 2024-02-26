chrome.browserAction.onClicked.addListener((currentTab) => {
    const videoIdMatch = currentTab.url.match(/youtube\.com\/.*[?&]v=([^&]+)/);

    if (videoIdMatch) {
        const videoId = videoIdMatch[1];
        const thumbnailURL = `https://img.youtube.com/vi/${videoId}/maxresdefault.jpg`;

        chrome.tabs.create({ url: thumbnailURL, index: currentTab.index + 1 });
    }
});
