package src;
import java.io.*;
import java.net.*;
import java.util.*;

//Thread de maintien en vie avec le gestionnaire
class IMOKThread extends Thread implements Runnable{
    private Socket gestionnaire;
    private BufferedReader lecteur;
    private PrintWriter ecrivain;
    //champ mess doit être généré avec la méthode enregistrementGestionnaire() de Diffuseur.java
    private String mess;

    public IMOKThread(Socket s, String enregistrement_mess){
        this.mess = enregistrement_mess;
        this.gestionnaire = s;
        try{
            this.lecteur = new BufferedReader(new InputStreamReader(this.gestionnaire.getInputStream()));
            this.ecrivain = new PrintWriter(new OutputStreamWriter(this.gestionnaire.getOutputStream()));
        }catch(Exception e){
            System.out.println("Erreur lors de la création du thread d'enregistrement");
            e.printStackTrace();
        }
    }

    public void run(){
        try{
            //enregistrement avec le gestionnaire via REGI
            ecrivain.print(this.mess);
            ecrivain.flush();

            //double readLine() pour récupérer REOK et RUOK 
            String reponse = lecteur.readLine();
            String msg = lecteur.readLine();

            //pour éviter des nullPointeur
            if (reponse == null || msg == null){
                return;
            }

            if(reponse.equals(Diffuseur.REOK)){
                while (true){
                    try{
                        //envoie de la réponse suite à RUOK reçu
                        if (msg.equals(Diffuseur.RUOK)){
                            ecrivain.print(Diffuseur.IMOK + "\r\n");
                            ecrivain.flush();
                        }
                        
                        //attente du prochain message du gestionnaire
                        msg = lecteur.readLine();
                        if (msg == null){
                            return;
                        }
                    }catch(Exception e){
                        e.printStackTrace();
                        return;
                    }
                }
            } else if(reponse.equals(Diffuseur.RENO)){
                System.out.println("[Erreur] : enregistrement avec le gestionnaire échoué");
            } else {
                System.out.println("[Erreur] : mauvais format de retour après l'enregistrement");
            }
        } catch(Exception e) {
            System.out.println("Erreur lors de l'enregistrement auprès du gestionnaire");
        }
    }
}


public class Diffuseur{
    private String identifiant;
    private int portMsg;
    private String multiDiff;
    private int portMultiDiff;
    //private LinkedList<String> listMsg;
    /*private DiffuseMulticast live;
    private EcouteUtilisateur ecoute;*/

    //Champs de constantes pour modularité du code
    public static final String ACKM = "ACKM";
    public static final String ENDM = "ENDM";
    public static final String LAST = "LAST";
    public static final String DIFF = "DIFF";
    public static final String MESS = "MESS";
    public static final String OLDM = "OLDM";
    public static final String REGI = "REGI";
    public static final String RUOK = "RUOK";
    public static final String IMOK = "IMOK";
    public static final String REOK = "REOK";
    public static final String RENO = "RENO";
    public static final int TAILLEMAXMSG = 140;
    public static final int TAILLEID = 8;
    public static final int FORMATPORT = 9999;

    public Diffuseur(String id, int portRecv, int portDiff, String addressDiff){
        if (portDiff > FORMATPORT || portRecv > FORMATPORT || id.length() > TAILLEID){
            System.out.println("[Erreur] : Impossible d'assembler le diffuseur, rappel : le numéro des ports doit être inférieur à 9999 et l'identifiant ne doit faire plus de 8 caractères\n");
            System.exit(1);
        }
        
        this.identifiant = formatageString(id, TAILLEID);
        this.portMsg = portRecv;
        this.multiDiff = addressDiff;
        this.portMultiDiff = portDiff;
        //this.listMsg = new LinkedList<String>();
    }

    public String getIdentifiantDiff(){
        return this.identifiant;
    }

    /*public void setEcoute(EcouteUtilisateur eu){
        this.ecoute = eu;
    }*/

    //Rajoute # a une string jusqu'à ce que sa taille atteigne iterFormat
    public static String formatageString(String s, int iterFormat){
        for(int i = s.length(); i < iterFormat; i++){
            s = s + "#";
        }
        return s;
    }

    //Rajoute des 0 devant le nombre pour qu'il soit encodé sous 4 bits
    public static String formatageEntier(int nb){
        int i = nb % 10000;
        if(i < 10){
            return "000"+i;
        } else if(i < 100){
            return "00"+i;
        } else if(i < 1000){
            return "0"+i;
        } else {
            return String.valueOf(i);
        }
    } 

    //Assemble un message qui sera ajouté dans la liste de message du diffuseur
    public String assembleMsgDiff(String id, String message){
        //Ce cas ne devrait jamais être atteint, est une sécurité
        if (message.length() > TAILLEMAXMSG || id.length() > TAILLEID){
            return "ERREUR## " + formatageString("ERREUR", TAILLEMAXMSG) + "\r\n";
        }

        return id + " " + formatageString(message, TAILLEMAXMSG) + "\r\n";
    }

    //Assemble un message d'enregistrement auprès du gestionnaire
    private String assembleMsgEnregistrement(String ip2){
        return REGI + " " + identifiant + " " + multiDiff + " " + portMultiDiff + " " + ip2 + " " + portMsg + "\r\n";
    }

    //Démarre un thread pour s'enregistrer auprès du gestionnaire et se maintenir en vie
    private boolean enregistrementGestionnaire(int portGestionnaire, String adressGestionnaire, String adressDiff){
        try{    
            Socket gestio = new Socket(adressGestionnaire, portGestionnaire);
            String mess = assembleMsgEnregistrement(adressDiff);
            IMOKThread t = new IMOKThread(gestio, mess);
            t.start();
            return true;
        } catch(Exception e) {
            return false;
        }
    }

    /*Obselète, le Diffuseur n'utilise pas sa liste de message 
    public void addToList(String msg){
        this.listMsg.add(msg);
    }*/

    //Convertit un string en entier et gère les problèmes d'exception
    public static int stringToInt(String s){
        int i = -1;
        try{
            i = Integer.valueOf(s);
        } catch (NumberFormatException n){
            System.out.println("[Erreur] : Le nombre n'est pas au bon format");
            System.exit(1);
        }
        return i;
    }

    /*Champs supposés de args : 
     * [0] : ID
     * [1] : port multidiffusion
     * [2] : port écoute
     * [3] : adresse multidiffusion
     * [4] : port gestionnaire
     * [5] : adresse gestionnaire
     * [6] : adresse de la machine où le diffuseur est présent
     * [7] : nom du fichier contenant les messages du diffuseur (optionnel)
     * 
     * Les adresses devront être écrites sans omettre les 0 pour 
     * que l'enregistrement auprès du gestionnaire n'échoue pas
     * Ex : java src/Diffuseur joker123 5151 5252 225.010.020.030 4242 127.000.000.001 127.000.000.001
     * Rajouter <bubs.txt> ou n'importe quel autre fichier a la fin pour initialiser vos propres messages
     */
    public static void main(String [] args){
        if(args.length < 7){
            System.out.println("[Erreur] : Pas assez d'arguments");
            System.exit(1);
        }

        if(args[3].length() != 15 || args[5].length() != 15 || args[6].length() != 15){
            System.out.println("[Erreur] : Les adresses ne sont pas dans le bon format");
            System.exit(1);
        }

        //Initialisation des variables
        LinkedList<String> msgDiff = new LinkedList<String>();

        DiffuseMulticast dm = new DiffuseMulticast(msgDiff, stringToInt(args[1]), args[3]);
        Diffuseur d = new Diffuseur(args[0], stringToInt(args[2]), stringToInt(args[1]), args[3]);
        EcouteUtilisateur eu = null;
        dm.setDiffuseur(d);

        //Phase d'enregistrement
        boolean test = d.enregistrementGestionnaire(stringToInt(args[4]), args[5], args[6]);
        if(!test){
            System.out.println("Enregistrement avec le gestionnaire échoué");
        }

        //Initialisation de la liste de message si un fichier a été précisé 
        boolean initial = true;
        if(args.length >= 8){
            try{
                File fileMsg = new File(args[7]);
                Scanner sc = new Scanner(fileMsg);
                while (sc.hasNextLine()) {
                    String data = sc.nextLine();
                    dm.ajoutMsg(d.assembleMsgDiff(args[0], data));
                }
                sc.close();
            } catch(FileNotFoundException e) {
                System.out.println("[Erreur] : le fichier des messages n'existe pas");
                //e.printStackTrace();
                initial = false;
            }
        }

        if(args.length == 7 || !initial){
            //Initialisation par défaut de la liste de message 
            dm.ajoutMsg(d.assembleMsgDiff(args[0], "The apocalypse shall soon be realised..."));
            dm.ajoutMsg(d.assembleMsgDiff(args[0], "Are there any grounds for that boldness of yours?"));
            dm.ajoutMsg(d.assembleMsgDiff(args[0], "Oh...?"));
            dm.ajoutMsg(d.assembleMsgDiff(args[0], "Is this no longer useful..."));
            dm.ajoutMsg(d.assembleMsgDiff(args[0], "I've no intention of stopping here."));
            dm.ajoutMsg(d.assembleMsgDiff(args[0], "Cast away that flesh, onward to the next dimension..."));
            dm.ajoutMsg(d.assembleMsgDiff(args[0], "I'll be using your power. Paradise Lost!"));
            dm.ajoutMsg(d.assembleMsgDiff(args[0], "I wonder if I can still draw out more efficiency..."));
            dm.ajoutMsg(d.assembleMsgDiff(args[0], "Still putting up a fight...?!"));
            dm.ajoutMsg(d.assembleMsgDiff(args[0], "What an irritating lot..."));
            dm.ajoutMsg(d.assembleMsgDiff(args[0], "Be seated, and await your end!"));
            dm.ajoutMsg(d.assembleMsgDiff(args[0], "Tremble before the oblivion..."));
            dm.ajoutMsg(d.assembleMsgDiff(args[0], "Even the power of these twelve black wings...?!"));
            dm.ajoutMsg(d.assembleMsgDiff(args[0], "This doesn't bode well."));
            dm.ajoutMsg(d.assembleMsgDiff(args[0], "Have I been surpassed...?!"));
            dm.ajoutMsg(d.assembleMsgDiff(args[0], "So I have erred in my calculations..."));
        }
        
        //Début des threads
        try{
            ServerSocket server = new ServerSocket(stringToInt(args[2]));
            Thread stream = new Thread(dm);
            stream.start();
            while(true){
                Socket socket = server.accept();
                eu = new EcouteUtilisateur(socket, dm);
                eu.setDiffuseur(d);
                Thread ecoute = new Thread(eu);
                ecoute.start();
            }
        } catch(Exception e) {
            System.out.println(e);
            e.printStackTrace();
        }
    }
}