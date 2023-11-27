<?php
var_dump($_FILES);
if (isset($_FILES['file'])) {
    $target_dir = "/Users/axelfernandez/ecole42/cursus42/webserv/srcs/tools/website_1/depot/";
    $target_file = $target_dir . basename($_FILES['file']['name']);

    if (move_uploaded_file($_FILES['file']['tmp_name'], $target_file)) {
        echo "Le fichier a été téléchargé.";
    } else {
        echo "Erreur lors du téléchargement du fichier.";
    }
} else {
    echo "Aucun fichier n'a été téléchargé.";
}


?>

