const { app, BrowserWindow } = require('electron')
function createWindow () {
  const win = new BrowserWindow({
    width: 646,
    height: 528,
    webPreferences: {
        nodeIntegration: true,
        contextIsolation: false,
        crossOriginIsolated: false,
        nodeIntegrationInWorker: true
    },
    resizable: false
  })
  win.webContents.session.webRequest.onHeadersReceived((details, callback) => {
      details.responseHeaders['Cross-Origin-Opener-Policy'] = ['same-origin'];
      details.responseHeaders['Cross-Origin-Embedder-Policy'] = ['require-corp'];
      callback({ responseHeaders: details.responseHeaders });
  });
  win.openDevTools();
  win.loadFile('camera.html')
}
app.on("ready",() => {
  createWindow()
  app.on('activate', () => {
    if (BrowserWindow.getAllWindows().length === 0) {
      createWindow()
    }
  })
})

app.on('window-all-closed', () => {
    app.quit()
})