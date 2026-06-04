const loginPageButton =
    document.querySelector(
        "#loginPageButton"
    );

loginPageButton.addEventListener(
    "click",
    () =>
    {
        window.location.href =
            "login.html";
    }
);

const usernameInput =
    document.querySelector(
        "#username"
    );

const passwordInput =
    document.querySelector(
        "#password"
    );

const registerButton =
    document.querySelector(
        "#registerButton"
    );
registerButton.addEventListener(
    "click",
    register
);

let API_BASE = "";

async function loadConfig()
{
    const response =
        await fetch(
            "backend-url.json"
        );

    const config =
        await response.json();

    API_BASE =
        config.apiBase;
}

async function register()
{
    await loadConfig();
    const username = usernameInput.value.trim();
    const password = passwordInput.value;

    const response =
    await fetch(
            `${API_BASE}/register/${encodeURIComponent(username)}/${encodeURIComponent(password)}`,
        {
            method: "POST"
        }
    );
    const data =
    await response.json();
        alert(
        data.message
    );
}