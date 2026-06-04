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
async function login() {
    const username =
        usernameInput.value
            .trim();

    const password = passwordInput.value;
    const response =
        await fetch(
            `http://localhost:18080/login/${encodeURIComponent(username)}/${encodeURIComponent(password)}`,
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