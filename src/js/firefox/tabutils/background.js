chrome.commands.onCommand.addListener(function(command) {
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
    chrome.tabs.query({ currentWindow: true }, function(tabs) {
        let tabInfo = tabs.map(tab => `${tab.title}: ${tab.url}`).join('\n');
        writeToClipboard(tabInfo);
    });
}

function pasteTabs()
{
    readFromClipboard(function(clipboardData) {
        let urls = clipboardData.split('\n');
        urls.forEach(url => {
            if (url.trim() !== '') {
                chrome.tabs.create({ url: url, active: true });
            }
        });
    });
}

function copyCurrentTab()
{
    chrome.tabs.query({ currentWindow: true, active: true }, function(tabs) {
        let tabInfo = `${tabs[0].title}: ${tabs[0].url}`;
        writeToClipboard(tabInfo);
    });
}

function pasteNextTab()
{
    readFromClipboard(function(clipboardData) {
        let urls = clipboardData.split('\n');
        chrome.tabs.query({ currentWindow: true, active: true }, function(currentTabs) {
            let currentTab = currentTabs[0];
            let currentIndex = currentTab.index;
            for (let i = 0; i < urls.length; i++) {
                chrome.tabs.create({ url: urls[i], index: currentIndex + i + 1, active: true });
            }
        });
    });
}

function writeToClipboard(data)
{
    document.addEventListener('copy', function(e) {
        e.clipboardData.setData('text/plain', data);
        e.preventDefault();
    });
    document.execCommand('copy');
}

function readFromClipboard(callback)
{
    let textArea = document.createElement("textarea");
    document.body.appendChild(textArea);
    textArea.focus();

    document.execCommand('paste');
    let clipboardData = textArea.value;

    let urls = clipboardData.match(/https?:\/\/[^\s]+/g) || [];

    callback(urls.join('\n'));

    document.body.removeChild(textArea);
}