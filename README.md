# smartcan C8Y ui plugin

Doc C8Y : https://www.cumulocity.com/guides/web/introduction/

1. Cloner le projet m2m-cumulocity-ui-plugin-examples
2. Modifier package.json pour faire pointer sur l'emplacement des libs C8Y : "dependencies": { "cumulocity-ui-build": "...\\8.15.3.tar.gz" }
3. Modifier cumulocity.json pour adapter avec le nom de l'appli et les imports nécessaires
4. Dans le répertoire plugins, créer un dossier "monplugin" (en copiant myBranding)
5. Dans le répertoire "monplugin", modifier cumulocity.json pour mettre le nom du plugin
6. Dans le répertoire "monplugin", modifier index.js pour créer le module angular
7. Dans le répertoire "monplugin", modifier les fichiers less, img etc pour customizer le plugin
8. Revenir dans le répertoire racine (du clone)
9. Lancer la commande c8y build:plugin monplugin plugins\monplugin
10. Récupérer le zip généré et l'installer dans l'appli (en supprimant tout autre plugin de branding) via l'interface C8Y