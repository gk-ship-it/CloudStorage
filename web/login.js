const registerPageButton =
    document.querySelector(
        "#registerPageButton"
    );
registerPageButton.addEventListener(
    "click",
    () =>
    {
        window.location.href =
            "register.html";
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

const loginButton =
    document.querySelector(
        "#loginButton"
    );
loginButton.addEventListener(
    "click",
    login
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

async function login() {
    await loadConfig();
    const username =
        usernameInput.value
            .trim();

    const password = passwordInput.value;
    const response =
        await fetch(
            `${API_BASE}/login/${encodeURIComponent(username)}/${encodeURIComponent(password)}`,
            {
                method: "POST"
            }
        );
    const data =
        await response.json();
    if(!data.success)
    {
        alert(
            data.message
        );
    
        return;
    }
    localStorage.setItem(
        "token",
        data.token
    );
    window.location.href =
    "index.html";
    
}