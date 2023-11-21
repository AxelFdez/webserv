<?php
session_start();

if (isset($_POST['username']) && isset($_POST['password'])) {
    $username = $_POST['username'];
    $password = $_POST['password'];

    // Ici, vérifiez les identifiants (exemple simplifié)
    if ($username == "user" && $password == "password") {
        // Génération et stockage du jeton
        $_SESSION['token'] = bin2hex(random_bytes(32));
        setcookie("token", $_SESSION['token'], time() + 3600);

        header("Location: protected_page.php"); // Rediriger vers la page protégée
        exit;
    } else {
        echo "Identifiants incorrects.";
    }
}
?>

<form method="post" action="login.php">
    Username: <input type="text" name="username"><br>
    Password: <input type="password" name="password"><br>
    <input type="submit" value="Login">
</form>