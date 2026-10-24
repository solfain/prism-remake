# Microsoft sign-in (OAuth)

Minecraft: Java Edition accounts are Microsoft accounts, and SOL Launcher signs you in with Microsoft's OAuth 2.0.
Microsoft requires every application that does this to have its **own application (client) ID**, registered by whoever
distributes it. SOL Launcher does not ship Prism Launcher's ID (see [FORK_NOTES.md](FORK_NOTES.md)), so a build without
an ID of its own cannot sign in, and without a Microsoft account you cannot add any account at all: offline accounts can
only be added once a Microsoft account that owns Minecraft exists (upstream's rule, kept on purpose).

There are two ways to fix that, depending on who you are.

## If you distribute SOL Launcher: register once, ship it in every build

1. [Register the application](#registering-the-application) and note its **Application (client) ID**.
2. [Ask Mojang to approve it](#mojang-approval) (needed before Minecraft accepts the sign-in).
3. Give the ID to the build. In the GitHub repository open **Settings > Secrets and variables > Actions > Variables** and
   add a repository variable named `SOL_MSA_CLIENT_ID` with the ID as its value (or run
   `gh variable set SOL_MSA_CLIENT_ID --body <the id>`). It is a public identifier, not a secret.
4. Push a commit or re-run the **Build** workflow. The Windows installer and portable zip of the new *preview* release
   have sign-in enabled, and nobody who installs them ever sees the setup window.

For a local build pass it to CMake instead: `cmake ... -DLauncher_MSA_CLIENT_ID=<the id>`.

## If you just run a copy that has no ID: use the guided setup

Choose **Add Microsoft account...** (in the sidebar's *Accounts* menu, or **Settings > Accounts > Add Microsoft**). A
window walks through the same registration (below), shows the two redirect URIs with copy buttons, and takes the
resulting ID. It is saved to **Settings > APIs > Microsoft Authentication client ID**, where you can change or clear it,
and the normal sign-in starts right away.

## Registering the application

In the [Microsoft Entra admin center](https://entra.microsoft.com/#view/Microsoft_AAD_RegisteredApps/ApplicationsListBlade)
(free, any Microsoft account works; an Azure subscription is not needed):

1. **App registrations > New registration.**
2. **Name:** `SOL Launcher` (users see this name on the consent screen).
3. **Supported account types:** *Personal Microsoft accounts only*. The launcher signs in through the `consumers`
   endpoint, so work and school accounts are not used.
4. **Redirect URI:** platform *Public client/native (mobile & desktop)*, value `http://localhost`. Register it. Microsoft
   ignores the port for loopback addresses, which is what the launcher's local sign-in listener relies on.
5. Open **Authentication**:
   - add a second redirect URI, `sollauncher://oauth/microsoft` (the launcher's own URL scheme; the installer registers
     it, a portable copy falls back to the loopback address);
   - under **Advanced settings** set **Allow public client flows** to **Yes** (the device-code sign-in needs it) and save.
6. On the **Overview** page copy the **Application (client) ID**.

You do **not** need a client secret, and you must never put one into the launcher: it is a public client.

The sign-in dialog runs two OAuth flows at once and uses whichever finishes first: the browser flow
(*authorization code*, redirecting to `http://localhost` or to `sollauncher://oauth/microsoft`) and the *device code*
flow (a short code you enter at microsoft.com/link, also shown as a QR code). Both ask for the scopes
`XboxLive.SignIn XboxLive.offline_access`; the launcher then exchanges the token with Xbox Live and Minecraft services
as the upstream code does.

## Mojang approval

Microsoft sign-in succeeding is not enough: Mojang only lets approved applications talk to the Minecraft services.
Request approval for your application ID at <https://aka.ms/AppRegInfo>. Until it is approved the sign-in completes at
Microsoft and then stops at the Minecraft step with **Invalid app registration**. That is not a bug in the launcher or in
your registration, it is the pending approval.

## Troubleshooting

| What you see | Cause and fix |
| --- | --- |
| `AADSTS50011` (reply URL does not match) | The redirect URI is missing, or was added under a *Web* platform. Register `http://localhost` and `sollauncher://oauth/microsoft` under *Public client/native*. |
| `AADSTS700016` (application not found) | A typo in the ID, or the app does not support personal Microsoft accounts. Check step 3. |
| `AADSTS7000218` (client secret or assertion required) | **Allow public client flows** is off. Turn it on (step 5). |
| *Invalid app registration* after signing in | Mojang's approval is still pending, see above. |
| Signed in, but "does not own Minecraft" | The Microsoft account has no Minecraft: Java Edition licence. |
| The browser flow opens but never returns | Use the code from the dialog instead (device code), or check that a firewall is not blocking `localhost`. |

## Where things are stored

The access and refresh tokens are kept in `accounts.json` in the launcher's data folder. Treat that file like a password
and do not share it or commit it. Removing an account in **Settings > Accounts** deletes its tokens from it.
