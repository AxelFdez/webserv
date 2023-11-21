<?php
session_start();

// Vérifier si le jeton de session correspond au jeton du cookie
if (!isset($_SESSION['token']) || $_SESSION['token'] !== $_COOKIE['token']) {
    exit('Accès non autorisé');
}

// Afficher l'image
echo '<img src="img.png" alt="Image Protégée">';
