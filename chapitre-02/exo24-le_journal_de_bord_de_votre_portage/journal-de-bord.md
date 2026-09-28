Symptôme : export : Le terme «export» n'est pas reconnu comme nom d'applet de commande, fonction, fichier de script ou programme exécutable.
J'ai cru : que je ne pouvais pas utiliser cette commande
C'était : une commande bash tapée dans PowerShell, qui ne la reconnaît pas.
Temps perdu : 5 minutes

Symptôme : Keytool failed: Generating 2 048 bit RSA key pair and self-signed certificate...
J'ai cru : que le JDK n'était pas correctement installé
C'était : PowerShell ne développe pas le ~ pour un programme externe, et Jenga ne le développe pas non plus de son côté. La clé partait vers un dossier ~ littéral qui n'existait pas.
Temps perdu : 10 minutes

Symptôme : Cannot create builder: No suitable toolchain found for Android arm64.
J'ai cru : avoir mal réglé une variable d'environnement
C'était : le SDK et le NDK n'étaient pas encore installés avec sdkmanager, seulement les command line tools et adb.
Temps perdu : 15 minutes

Symptôme : l'apk produit se trouve dans android-build-x86_64, alors que le téléphone est en arm64.
J'ai cru : que androidabis(["arm64-v8a"]) suffisait à choisir l'architecture
C'était : jenga package ne lit pas les architectures du projet quand --platform ne précise pas d'architecture, il prend celle du PC qui lance la commande.
Temps perdu : 10 minutes

Symptôme : apksigner not found. Install Android SDK build-tools.
J'ai cru : que le SDK n'était pas complet ou que le chemin était mal réglé
C'était : jenga sign cherche apksigner.exe sous Windows, alors que le vrai fichier du SDK est apksigner.bat.
Temps perdu : 5 minutes