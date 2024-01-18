chrome.commands.onCommand.addListener((command) => {
    if (command === "copyTabs") {
        copyTabs();
    } else if (command === "pasteTabs") {
        pasteTabs();
    } else if (command === "copyCurrentTab") {
        copyCurrentTab();
    } else if (command === "pasteNextTab") {
        pasteNextTab();
    }
});

function copyTabs()
{
    chrome.tabs.query({ currentWindow: true }, (tabs) => {
        const tabInfo = tabs.map(tab => `${tab.title}: ${tab.url}`).join('\n');
        writeToClipboard(tabInfo);
    });
}

function pasteTabs()
{
    readFromClipboard((clipboardData) => {
        const urls = clipboardData.split('\n');
        for (const url of urls) {
            if (url.trim() !== '') {
                chrome.tabs.create({ url: url, active: true });
            } else {
                chrome.tabs.create({ url: 'about:blank', active: true });
            }
        }
    });
}

function copyCurrentTab()
{
    chrome.tabs.query({ currentWindow: true, active: true }, (tabs) => {
        const tabInfo = `${tabs[0].title}: ${tabs[0].url}`;
        writeToClipboard(tabInfo);
    });
}

function pasteNextTab()
{
    readFromClipboard((clipboardData) => {
        const urls = clipboardData.split('\n');
        chrome.tabs.query({ currentWindow: true, active: true }, (currentTabs) => {
            const currentTab = currentTabs[0];
            const currentIndex = currentTab.index;
            for (let i = 0; i < urls.length; i++) {
                chrome.tabs.create({ url: urls[i], index: currentIndex + i + 1, active: true });
            }
        });
    });
}

function writeToClipboard(data)
{
    document.addEventListener('copy', (e) => {
        e.clipboardData.setData('text/plain', data);
        e.preventDefault();
    });
    document.execCommand('copy');
}

function readFromClipboard(callback)
{
    const textArea = document.createElement("textarea");
    document.body.appendChild(textArea);
    textArea.focus();

    document.execCommand('paste');
    const clipboardData = textArea.value;
    const urls = clipboardData.split("\n").map(
        (line) => line.split(/(\s+)/).at(-1).replace(/^about:.*$/, "") || "");

    callback(urls.join('\n'));

    document.body.removeChild(textArea);
}
