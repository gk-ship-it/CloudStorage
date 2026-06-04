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

async function register()
{
    const username = usernameInput.value.trim();
    const password = passwordInput.value;

    const response =
    await fetch(
        `http://localhost:18080/register/${encodeURIComponent(username)}/${encodeURIComponent(password)}`,
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