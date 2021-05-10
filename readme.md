# Projet NetRadio Groupe 13

## Membres :
PHOL ASA Rémy
CHHOA Thierry
KHOW Antoine

## Compilation :
Il suffit de rentrer `make` sur le terminal à la racine du projet pour le compiler
Pour supprimer les fichiers compilés vous pouvez entrer `make clean`

## Exécution :
Pour exécuter le gestionnaire, après compilation, entrez `./gestionnaire XXXX` où `XXXX` est le numéro du port

Pour exécuter le diffuseur, il faut au préalable exécuter le gestionnaire depuis un autre terminal (pour l'enregistrement 
uniquement, le diffuseur continuera de fonctionner même s'il n'est pas enregistré).
L'enregistrement se fait lors de l'exécution, il faut entrer `java src/Diffuseur identifiant portMultidiffusion portEcoute adresseMultidiff portGestionnaire adresseGestionnaire adresseMachineDuDiffuseur pathFile`
Détails de la commande :
[0] : `identifiant` -> ID du diffuseur
[1] : `portMultidiffusion` -> port multidiffusion du diffuseur
[2] : `portEcoute` -> port écoute du diffuseur
[3] : `adresseMultidiff` -> adresse multidiffusion du diffuseur
[4] : `portGestionnaire` -> port du gestionnaire
[5] : `adresseGestionnaire` -> adresse du gestionnaire
[6] : `adresseMachineDuDiffuseur` -> adresse de la machine où le diffuseur est présent
[7] : `pathFile` -> nom du fichier contenant les messages du diffuseur (optionnel)
Le diffuseur lira le fichier `pathFile` ligne par ligne, 1 ligne = 1 message de la liste

Pour exécuter le client, après compilation, entrez `./client`
Le client demandera d'abord votre identifiant d'au plus 8 caractères et des informations variables au cours de l'exécution selon 
vos demandes. Respectez bien ce qui est indiqué entre `[]`.
Les opérations du client sont :
LIST -> Demande à un gestionnaire de diffuser la liste de ses diffuseurs
MESS -> Envoie un message à un diffuseur pour qu'il puisse le retransmettre
LAST -> Demande à un diffuseur la liste de ses derniers messages
HEAR -> Ecoute dans un port de multidiffusion
HELP -> Affiche l'aide pour l'utilisateur
EXIT -> Termine le programme

## Exemples d'exécutions :
Gestionnaire : `./gestionnaire 4646`
Diffuseur (en prenant en compte que le gestionnaire et le diffuseur sont exécutés sur lulu)
- Sans fichier message : `java src/Diffuseur Joker123 8855 5454 225.010.020.030 4646 192.168.070.236 192.168.070.236`
- Avec fichier message : `java src/Diffuseur Beel 7766 5858 225.030.020.010 4646 192.168.070.236 192.168.070.236 bubs.txt`
Attention, les adresses doivent faire exactement 15 caractères sinon le programme ne se lancera pas

## Architecture du code :
Le projet a été divisé en 3 grandes parties :
- En C le gestionnaire et le client
- En java le diffuseur

### Gestionnaire
Le gestionnaire est composé d'une liste de diffuseur qu'il consulte ou modifie en ajoutant de nouveaux diffuseurs en fonction des
besoins. Il possède un thread principal qui crée des threads secondaires pour chaque communication effectuées sur celui-ci.
Chacun de ces threads secondaires gère une fonctionnalité avec soit un client, soit un diffuseur.

### Client
Le client est constitué d'un ensemble de fonctions qui permettent de se connecter à un port et d'envoyer le message voulu.
Il contient également une interface textuelle pour faciliter la communication entre les différentes composantes.

### Diffuseur
Le diffuseur se compose en 3 fichiers sources, le fichier `DiffuseMulticast.java` contenant un thread qui gère le multicast en UDP,
le fichier `EcouteUtilisateur.java` contenant également un autre thread qui reste en écoute sur un port donné pour traiter
les opérations du client (Ex : MESS, LAST) et le fichier `Diffuseur.java` qui contient le `main` du diffuseur et va construire
les 2 threads mentionnés plus tôt. Il est également doté d'un thread pour la communication avec le gestionnaire qui est lancé dès
l'exécution du diffuseur.

## Extension :
Un client est autorisé à envoyer un message à un gestionnaire qui devra le transmettre à tous ses diffuseurs. 
Il suffit d'utiliser la commande MESS du client et d'indiquer l'adresse et le port du gestionnaire.

## Tests avec autre groupe :
Notre projet a été testé sur lulu avec le groupe 19 (groupe de MARIN Grégoire), les intéractions entre chaque composantes 
semblaient fonctionner correctement.

## Bugs connus :
Le seul bug connu à ce jour est suite à l'opération `LAST` du client, si le client demande suffisamment de messages pour demander 
le tout premier message du diffuseur, celui ci ne sera jamais envoyé au client (Exemple : si un client demande 300 messages et que 
le diffuseur n'a diffusé que 30 messages, il ne renverra que 29 messages, le message numéro 0000 ne sera jamais renvoyé pour une 
raison inconnue).