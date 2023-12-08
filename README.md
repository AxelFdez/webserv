# Webserv

## Introduction

Le projet Webserv consiste en l'écriture d'un serveur HTTP en C++ 98. Ce serveur permet de gérer les requêtes HTTP et de servir des pages Web aux clients. Il se base sur le protocole HTTP (Hypertext Transfer Protocol) qui est essentiel pour la communication de données sur le World Wide Web.

## Fonctionnalités

- **Serveur HTTP complet :** Webserv est capable de gérer des requêtes HTTP provenant de navigateurs web et de renvoyer des réponses appropriées.
- **Fichier de configuration :** Vous pouvez configurer le serveur en utilisant un fichier de configuration pour définir des ports, des routes, des pages d'erreur, etc.
- **Gestion des méthodes HTTP :** Webserv prend en charge les méthodes GET, POST, et DELETE.
- **Non bloquant :** Le serveur est non bloquant, ce qui garantit qu'il reste disponible même en cas de charge élevée. Fonction de multiplexing utilisée pour gérer les connexions simultanées.
- **Prise en charge des fichiers téléchargés :** Les clients peuvent télécharger des fichiers depuis le serveur.

## Configuration

Pour utiliser Webserv, vous pouvez créer un fichier de configuration personnalisé pour définir les paramètres du serveur.

Exemple de fichier de configuration :

	server    [

		host    127.0.0.1;
		listen   8080;
		server_name localhost;
		body_size_max 10000;
		#root [define root of your website];

		location   /        {
			methods         GET POST;
			index           index.html;
			#download            on;
			#download_folder     [define root of deposit];
		}

		location  side_pages/            {
		#   root [define root of this location];
			index  side_page.html index.htm;
			methods GET POST DELETE;
			directory_listing      on;
		}

		#error_404              [define root of error page]/404.html;

	]

## Compilation

Utilisez le Makefile fourni pour compiler le projet :

```shell
make
```

## Utilisation

Pour exécuter le serveur Webserv, utilisez la commande suivante :

```shell
./webserv [fichier_de_configuration]
```

ou sans argument pour prendre le fichier de configuration par défaut :

```shell
./webserv
```

## Auteur

Ce projet a été développé par Axel et Christophe.

