<?php
if (isset($_FILES['file'])) {
    $target_dir = "/Users/chris/Desktop/webServ42/srcs/tools/website_1/depot/";
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

