<?php
if (isset($_FILES['fileToUpload'])) {
    $target_dir = "/Users/axelfernandez/ecole42/cursus42/webserv/srcs/tools/depot/";
    $target_file = $target_dir . basename($_FILES['fileToUpload']['name']);

    if (move_uploaded_file($_FILES['fileToUpload']['tmp_name'], $target_file)) {
        echo "Le fichier a été téléchargé.";
    } else {
        echo "Erreur lors du téléchargement du fichier.";
    }
} else {
    echo "Aucun fichier n'a été téléchargé.";
}
?>
