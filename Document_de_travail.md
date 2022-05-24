# 09.05.22

## setup git

cd ~/LinEmb/projects
git clone https://gitlab-etu.ing.he-arc.ch/isc/2021-22/niveau-2/2247.1-linux-emb-il/gr9.git

## 1. tester drvTest

### Commands

#### Crée le dossier pour le driver

```shell
cd ~/LinEmb/pi4-config/package
mkdir drvTest
cd drvTest
```

#### Crée les fichier de config du driver

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

#### Ajouter le module à la configuration de buildroot

```shell
cd ~/LinEmb/pi4-config/

# le fichier Config.in est déjas crée avec labo5.d
sed -i '$ i\source "$BR2_EXTERNAL_PI4_CONFIG_PATH/package/drvTest/Config.in"' Config.in

```

#### Menu config

```shell
cd ../build-pi4/
make menuconfig
```

Dans le menu `External options -> Custom packages` cocher `drvTest`. Sauvegarder et quitter.

#### Compiler le noyau

```shell
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

## Lancer une application Qt GUI avec VNC

Après avoir copié l'exécutable sur le RPI :

```shell
./qt-server -platform vnc -geometry 500x500 -plugin evdevmouse &
```

Puis, depuis la machine hôte, se connecter au RPI avec VNC sur le port 5900.

