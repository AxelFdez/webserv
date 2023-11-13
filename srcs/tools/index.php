<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Connexion</title>
    <link rel="stylesheet" href="style.css">
</head>
<body>
    <div class="login-container">
        <h1>Connexion</h1>
        <form action="traitement.php" method="post">
            <div class="form-group">
                <label for="id">Identifiant :</label>
                <input type="text" id="id" name="id">
            </div>
            <div class="form-group">
                <label for="password">Mot de passe :</label>
                <input type="password" id="password" name="password">
            </div>
            <button type="submit">Se connecter</button>
        </form>
    </div>
</body>
</html>
