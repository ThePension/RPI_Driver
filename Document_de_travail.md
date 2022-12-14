# 09.05.22

## setup git

```shell
cd ~/LinEmb/projects

git clone https://gitlab-etu.ing.he-arc.ch/isc/2021-22/niveau-2/2247.1-linux-emb-il/gr9.git
```

## tester drvTest

### Commands

### Créer le dossier pour le driver

```shell
cd ~/LinEmb/pi4-config/package

mkdir drvTest

cd drvTest
```

### Créer les fichier de config du driver

```shell
cat << 'EOF' > Config.in
config BR2_PACKAGE_DRVTEST
    bool "drvTest"
    depends on BR2_LINUX_KERNEL
    help
        Hello test help
EOF

cat << 'EOF' > drvTest.desk
name: drvTest
EOF

cat << 'EOF' > drvTest.mk
DRVTEST_VERSION = 1.0
DRVTEST_MODULE_VERSION = 1.0
DRVTEST_SITE = ../projects/gr9/driver
DRVTEST_SITE_METHOD = local
DRVTEST_LICENSE = GPL-2.0
$(eval $(kernel-module))
$(eval $(generic-package))
EOF
```

### Ajouter le module à la configuration de buildroot

```shell
cd ~/LinEmb/pi4-config/

# le fichier Config.in est déja créé avec labo5.d
sed -i '$ i\source "$BR2_EXTERNAL_PI4_CONFIG_PATH/package/drvTest/Config.in"' Config.in

```

### Menu config

```shell
cd ../build-pi4/

make menuconfig
```

Dans le menu `External options -> Custom packages`, cocher `drvTest`. Sauvegarder et quitter.

### Compiler le noyau

```shell
 export BR2_EXTERNAL=../pi4-config/

 make -j8
```

# 16.05.2022

### Compiler un fichier source

```shell
 cd project/server

 make
```

### Envoie du fichier compilé sur le RPI

scp test_drvTest rpi@157.26.91.84:/home/rpi/prg

### Charger le module sur le rpi

```shell
ssh rpi@157.26.91.84 # Password : rpi

su # Password : root

modprobe drvTest
```

### Lancer le programme

```shell
cd /home/rpi/prg

su

./test_drvTtest
```

# 24.05.2022

## Compiler une application Qt

Créer un fichier .pro et ajouter son contenu, par exemple :

```
QT += network widgets

SOURCES += main.cpp \
            server.cpp
          
HEADERS += server.h
```

Pour créer le bon fichier <i>Makefile</i>, utiliser l’outil <i>qmake</i> de Qt qui se trouve dans le dossier généré <i>build-pi4/host/bin</i>.
Il faut le refaire à chaque que le fichier .pro est modifié.

```shell
~/LinEmb/build-pi4/host/bin/qmake qt-server.pro
```

Ensuite, pour compiler, entrer la commande

```shell
make
```

Une fois cela fait, nous avons créé et implémenté deux projets :
- Un serveur, qui envoie des données (simulées pour l'instant, plus par la suite) sur le réseau en cas de requête
- Un client, qui récupère les données demandées et les affiche sous forme de graphe

Ces implémentations se basent grandement sur les exemples de server/client disponibles sur la documentation de Qt.

Server : https://doc.qt.io/qt-6/qtnetwork-fortuneserver-example.html

Client : https://doc.qt.io/qt-6/qtnetwork-fortuneclient-example.html

Comme notre driver récupère les données de couleur, nous avons créée une classe, nommée 'Data', possédant les attributs suivants :
- luminosity
- red
- blue
- green

Nous avons rendu cette classe sérialisable, afin de pouvoir la transmettre sur le réseau et donc de faciliter le transport.

## Lancer une application Qt GUI avec VNC

Après avoir copié l'exécutable sur le RPI :

```shell
./qt-server -platform vnc -geometry 500x500 -plugin evdevmouse &
```

Puis, depuis la machine hôte, se connecter au RPI avec VNC sur le port 5900.

## Compiler le client sur la VM

```shell
sudo apt-get install qt5-qmake
sudo apt-get install qt5-default
sudo apt-get install libqt5charts5-dev
sudo apt-get install qtbase5-dev-tools
```

Regénérer le <i>Makefile</i> avec <i>qmake</i> :

```shell
qmake qt-client.pro
```

Compiler :

```shell
make clean
make
```

## Ajouter le driver dans l'image

Refaire les mêmes étapes qu'avec le driverTest pour ajouter le driver à l'image .

## Charger i2c

```shell
modprobe i2c-bcm2835

modprobe i2c-dev

modprobe drvI2C
```

# 30.05.2022

Modification du serveur :
- Transférer le serveur en mode console, pour ne plus devoir utiliser VNC
- Récupérer les données du driver au lieu de les simuler
- Améliorer l'interface graphique du client
- Résoudre les potentielles erreurs

# 09.05.2022

Modification du client :
- Ajout d'un thread qui récupère les données dans une boucle infinie, pour ne pas bloquer l'interface graphique
- Ajout d'un bouton "stop" pour arrêter le thread
- Aide de différents groupes (surtout Alessio Comi et Jeanne Michel)

# Conclusion

Notre driver permet de récupérer les données de couleur (luminosité, rouge, bleu, vert).

A chaque fois que notre serveur est interrogé, il récupère ces données, les stocke dans un buffer circulaire de taille 100, puis les envoie (le buffer entier) à la personne ayant effectué la requête.

Notre client, dans un thread séparé, interroge le serveur toutes les secondes, et affiche les données récupérées dans un graphique.

Tous nos codes sources se trouvent sur GitLab, à l'adresse suivante : https://gitlab-etu.ing.he-arc.ch/isc/2021-22/niveau-2/2247.1-linux-emb-il/gr9

# Sources

- Qt6, client fortune exemple : https://doc.qt.io/qt-6/qtnetwork-fortuneclient-example.html
- Qt6, server fortune exemple : https://doc.qt.io/qt-6/qtnetwork-fortuneserver-example.html