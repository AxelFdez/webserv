<?php
session_start();

if ($_SERVER['REQUEST_METHOD'] == 'POST') {
    if (isset($_POST['username']) && isset($_POST['password'])) {
        $username = $_POST['username'];
        $password = $_POST['password'];

        // Remplacer ceci par votre logique d'authentification
        if ($username == "user" && $password == "password") {
            $_SESSION['logged_in'] = true;
            header("Location: protected_page.php");
            exit;
        } else {
            echo "Identifiants incorrects.";
        }
    }
}
?>

<form method="post" action="login.php">
    Username: <input type="text" name="username"><br>
    Password: <input type="password" name="password"><br>
    <input type="submit" value="Login">
</form>
