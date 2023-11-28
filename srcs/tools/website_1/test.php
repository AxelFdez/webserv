<!DOCTYPE html>
<html>
<head>
    <title>Page PHP Simple</title>
</head>
<body>

<h1>Bienvenue sur ma page PHP</h1>

<?php
if (isset($_GET['nom'])) {
    $nom = $_GET['nom'];
    echo "Nom: " . htmlspecialchars($nom);
    setcookie("nom", $nom, time() + 3600);
} else {
    echo "La variable 'nom' par GET n'est pas définie.";
}
echo nl2br("\n");
if (isset($_POST['nom'])) {
    $nom = $_POST['nom'];
    echo "\nNom: " . htmlspecialchars($nom);
    setcookie("nom", $nom, time() + 3600);
} else {
    echo "\nLa variable 'nom' par POST n'est pas définie.";
}
echo nl2br("\n");
if (isset($_COOKIE['nom']))
{
    $nom = $_COOKIE['nom'];
    echo htmlspecialchars($nom) . "a été trouvé dans les cookies";
}
while (1) {
    echo "Je suis dans une boucle infinie";
}
?>
</body>
</html>
