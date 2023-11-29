<?php
session_start();

echo $_SESSION['logged_in'];

if (isset($_SESSION['logged_in']) && $_SESSION['logged_in'] == true) {
    echo '<img src="img.png" alt="Image Protégée">';
}
else {
    echo "Vous n'êtes pas connecté.";
}
?>
