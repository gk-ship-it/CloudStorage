const API_BASE = "http://localhost:18080";

const fileInput = document.querySelector("#fileInput");
const fileNameInput = document.querySelector("#fileNameInput");
const uploadButton = document.querySelector("#uploadButton");
const refreshButton = document.querySelector("#refreshButton");

const logoutButton =
    document.querySelector(
        "#logoutButton"
    );

const changePasswordButton =
    document.querySelector(
        "#changePasswordButton"
    );

let renameOldFilename = "";

logoutButton.addEventListener(
    "click",
    logout
);
changePasswordButton.addEventListener(
    "click",
    changePassword
);


const fileList = document.querySelector("#fileList");
const statusText = document.querySelector("#statusText");
const fileCount = document.querySelector("#fileCount");
const detailDialog = document.querySelector("#detailDialog");
const detailTitle = document.querySelector("#detailTitle");
const detailBody = document.querySelector("#detailBody");
const passwordDialog =
    document.querySelector(
        "#passwordDialog"
    );

const closePasswordDialogButton =
    document.querySelector(
        "#closePasswordDialogButton"
    );

const oldPasswordInput =
    document.querySelector(
        "#oldPasswordInput"
    );

const newPasswordInput =
    document.querySelector(
        "#newPasswordInput"
    );

const submitPasswordButton =
    document.querySelector(
        "#submitPasswordButton"
    );
const closeDialogButton = document.querySelector("#closeDialogButton");
const categoryButtons = document.querySelectorAll(".category-btn");
const searchInput =
  document.querySelector(
    "#searchInput"
  );

const renameDialog =
    document.getElementById(
        "renameDialog"
    );

const renameInput =
    document.getElementById(
        "renameInput"
    );

const submitRenameButton =
    document.getElementById(
        "submitRenameButton"
    );

const closeRenameDialogButton =
    document.getElementById(
        "closeRenameDialogButton"
    );


let allFiles = [];

let currentCategory =
  "all";
function getToken() {
  return localStorage.getItem(
    "token"
  );
}
if(!getToken())
{
    window.location.href =
        "login.html";
}

function setStatus(message) {
  statusText.textContent = message;
}

function encodeName(name) {
  return encodeURIComponent(name);
}

function isSafeName(name) {
  return Boolean(name) && !name.includes("/") && !name.includes("..");
}

function formatBytes(value) {
  const size = Number(value);

  if (!Number.isFinite(size)) {
    return "Unknown";
  }

  if (size < 1024) {
    return `${size} B`;
  }

  if (size < 1024 * 1024) {
    return `${(size / 1024).toFixed(1)} KB`;
  }

  return `${(size / 1024 / 1024).toFixed(1)} MB`;
}

function logout()
{
    localStorage.removeItem(
        "token"
    );

    window.location.href =
        "login.html";
}

function changePassword()
{
    oldPasswordInput.value =
        "";

    newPasswordInput.value =
        "";

    passwordDialog.showModal();
}
async function submitPasswordChange()
{
    const oldPassword =
        oldPasswordInput.value.trim();

    const newPassword =
        newPasswordInput.value.trim();
    if(
        !oldPassword ||
        !newPassword
    )
    {
        setStatus(
            "Fill both password fields"
        );
      
        return;
    }
    try
{
    const response =
        await fetch(
            `${API_BASE}/change-password/${encodeURIComponent(oldPassword)}/${encodeURIComponent(newPassword)}`,
            {
                method: "POST",

                headers:
                {
                    Authorization:
                        getToken()
                }
            }
        );

    const data =
        await response.json();
    
        setStatus(
        data.message
    );
        if(
    data.success
)
{
    alert(
        "Password changed successfully"
    );

    passwordDialog.close();

    oldPasswordInput.value =
        "";

    newPasswordInput.value =
        "";
}
    }
catch(error)
{
    setStatus(
        error.message
    );
}
    
}

function createButton(label, className, onClick) {
  const button = document.createElement("button");
  button.type = "button";
  button.className = className;
  button.textContent = label;
  button.addEventListener("click", onClick);
  return button;
}

function renderFiles(files) {
  fileList.textContent = "";
  fileCount.textContent = `${files.length} ${files.length === 1 ? "file" : "files"}`;

  if (files.length === 0) {
    const empty = document.createElement("div");
    empty.className = "empty-state";
    empty.textContent = "No files";
    fileList.append(empty);
    return;
  }

  files.forEach((file) => {
    const row = document.createElement("article");
    row.className = "file-row";

    const name = document.createElement("div");
    name.className = "file-name";
    name.title = file.filename;
    name.textContent = file.filename;

    const size = document.createElement("div");
    size.className = "file-meta";
    size.textContent = formatBytes(file.size);

    const uploaded = document.createElement("div");
    uploaded.className = "file-meta";
    uploaded.textContent = file.uploaded || "Unknown";

    const actions = document.createElement("div");
    actions.className = "actions";

    const download =
      createButton(
        "Download",
        "text-button",
        () => downloadFile(
          file.filename
        )
      );

    actions.append(
      createButton("Details", "text-button", () => showDetails(file.filename)),
      download,
      createButton("Rename", "text-button", () => renameFile(file.filename)),
      createButton("Delete", "danger-button", () => deleteFile(file.filename))
    );

    row.append(name, size, uploaded, actions);
    fileList.append(row);
  });
}

async function requestText(url, options = {}) {
  const response = await fetch(url, options);
  const text = await response.text();

  if (!response.ok) {
    throw new Error(text || `Request failed with ${response.status}`);
  }

  return text;
}

function filterFiles() {
  const search =
    searchInput.value
      .toLowerCase()
      .trim();

  let filtered =
    allFiles;

  if (
    currentCategory !== "all"
  ) {
    filtered =
      filtered.filter(
        file =>
          file.category ===
          currentCategory
      );
  }

  if (search) {
    filtered =
      filtered.filter(
        file =>
          file.filename
            .toLowerCase()
            .includes(search)
      );
  }

  renderFiles(
    filtered
  );
}

submitPasswordButton.addEventListener(
    "click",
    submitPasswordChange
);

submitRenameButton.addEventListener(
    "click",
    submitRename
);

closeRenameDialogButton.addEventListener(
    "click",
    () =>
    {
        renameDialog.close();
    }
);

async function loadFiles() {
  refreshButton.disabled = true;
  setStatus("Loading");

  try {
    const response = await fetch(
      `${API_BASE}/files`,
      {
        headers:
        {
          Authorization:
            getToken()
        }
      }
    );

    const files =
      await response.json();

    allFiles = files;

    filterFiles();

    setStatus("Ready");
  }
  catch (error) {
    renderFiles([]);

    setStatus(
      error.message
    );
  }
  finally {
    refreshButton.disabled = false;
  }
}

async function uploadFile() {
  const selectedFile = fileInput.files[0];

  if (!selectedFile) {
    setStatus("Choose a file");
    return;
  }

  const filename = (fileNameInput.value || selectedFile.name).trim();

  if (!isSafeName(filename)) {
    setStatus("Invalid filename");
    return;
  }

  uploadButton.disabled = true;
  setStatus("Uploading");

  try {
    await requestText(`${API_BASE}/upload/${encodeName(filename)}`,
      {
        headers:
        {
          Authorization:
            getToken()
        },

        method: "POST",
        body: selectedFile
      });

    fileInput.value = "";
    fileNameInput.value = "";
    setStatus("Uploaded");
    await loadFiles();
  } catch (error) {
    setStatus(error.message);
  } finally {
    uploadButton.disabled = false;
  }
}

async function showDetails(filename) {
  detailTitle.textContent = filename;
  detailBody.textContent = "Loading";
  detailDialog.showModal();

  try {
    const response = await fetch(
      `${API_BASE}/file/${encodeName(filename)}`,
      {
        headers:
        {
          Authorization:
            getToken()
        }
      });

    const file = await response.json();

    detailBody.textContent =
      `ID: ${file.id}\n` +
      `Filename: ${file.filename}\n` +
      `Path: ${file.path}\n` +
      `Size: ${file.size}\n` +
      `Uploaded: ${file.uploaded}`;
  }
  catch (error) {
    detailBody.textContent =
      error.message;
  }
}

function renameFile(oldName)
{
    renameOldFilename =
        oldName;

    renameInput.value =
        oldName;

    renameDialog.showModal();
}

async function submitRename()
{
    const cleanName =
        renameInput.value.trim();

    if(
        !isSafeName(
            cleanName
        )
    )
    {
        setStatus(
            "Invalid filename"
        );

        return;
    }

    setStatus(
        "Renaming"
    );

    try
    {
        await requestText(
            `${API_BASE}/rename/${encodeName(renameOldFilename)}/${encodeName(cleanName)}`,
            {
                headers:
                {
                    Authorization:
                        getToken()
                },

                method: "POST"
            }
        );

        renameDialog.close();

        setStatus(
            "Renamed"
        );

        await loadFiles();
    }
    catch(error)
    {
        setStatus(
            error.message
        );
    }
}

async function deleteFile(filename) {
  const confirmed = window.confirm(`Delete ${filename}?`);

  if (!confirmed) {
    return;
  }

  setStatus("Deleting");

  try {
    await requestText(`${API_BASE}/delete/${encodeName(filename)}`, {
      headers:
      {
        Authorization:
          getToken()
      },
      method: "POST"
    });
    setStatus("Deleted");
    await loadFiles();
  } catch (error) {
    setStatus(error.message);
  }
}

async function downloadFile(filename) {
  const response =
    await fetch(
        `${API_BASE}/download/${encodeName(filename)}`,
        {
            headers:
            {
                Authorization:
                    getToken()
            }
        }
    );
    const text =
    await response.text();

    const blob =
    new Blob(
        [text]
    );
    const url =
    URL.createObjectURL(
        blob
    );

const link =
    document.createElement(
        "a"
    );

link.href = url;

link.download =
    filename;

link.click();

URL.revokeObjectURL(
    url
);
}

fileInput.addEventListener("change", () => {
  const selectedFile = fileInput.files[0];

  if (selectedFile && !fileNameInput.value) {
    fileNameInput.value = selectedFile.name;
  }
});

uploadButton.addEventListener("click", uploadFile);
refreshButton.addEventListener("click", loadFiles);
closeDialogButton.addEventListener("click", () => detailDialog.close());
closePasswordDialogButton
    .addEventListener(
        "click",
        () =>
            passwordDialog.close()
    );



loadFiles();

categoryButtons.forEach(
  button => {
    button.addEventListener(
      "click",
      () => {
        currentCategory =
          button.dataset.category;

        filterFiles();
      }
    );
  }
);

searchInput.addEventListener(
  "input",
  filterFiles
);
