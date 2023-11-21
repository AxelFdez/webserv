<?php
// Exemple de traitement de formulaire
if ($_SERVER["REQUEST_METHOD"] == "POST") {
    $id = $_POST['id'];
    $password = $_POST['password'];
	setcookie("id", $id, time() + 3600);
	setcookie("pass", $password, time() + 3600);

    if ($id == "admin" && $password == "pass") {
		header("Location : img.png", true, 303);
        exit();
    } else {
        header("Location : index.html", true, 303);
        exit();
    }
}
?>
