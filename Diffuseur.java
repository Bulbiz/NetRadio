import java.io.*;
import java.net.*;
import java.util.*;

public class Diffuseur{
    private String identifiant;
    private int portMsg;
    private InetSocketAddress multiDiff;
    private int portMultiDiff;
    private LinkedList<String> listMsg;

    public int numMsg = 0;
    public static final String ACKM = "ACKM";
    public static final String ENDM = "ENDM";
    public static final String LAST = "LAST";
    public static final String DIFF = "DIFF";
    public static final String MESS = "MESS";
    public static final String OLDM = "OLDM";
    public static final String REGI = "REGI";
    public static final String RUOK = "RUOK";
    public static final String IMOK = "IMOK";
    public static final int TAILLEMAXMSG = 140;
    public static final int TAILLEID = 8;

    public Diffuseur(String id, int portRecv, int portDiff) throws Exception{
        if (portDiff < 9999 || portRecv < 9999 || id.length() > 8){
            throw new Exception("[Erreur] : Impossible d'assembler le diffuseur, rappel : le numéro des ports doit être inférieur à 9999 et l'identifiant ne doit faire plus de 8 caractères\n");
        }
        
        this.identifiant = id;
        this.portMsg = portRecv;
        this.multiDiff = new InetSocketAddress(portDiff);
        this.portMultiDiff = portDiff;
        this.listMsg = new LinkedList<String>();
    }

    /*public Diffuseur(String id, DatagramSocket recv, InetSocketAddress multiDiff, DatagramSocket portDiff) throws Exception{
        if (id.length() > 8){
            throw new Exception("[Erreur] : Impossible d'assembler le diffuseur, rappel : l'identifiant ne doit faire plus de 8 caractères\n");
        }
        
        this.identifiant = id;
        this.portMsg = recv;
        this.multiDiff = multiDiff;
        this.portMultiDiff = portDiff;
        this.listMsg = new LinkedList<String>();
    }*/

    //TODO: bricoler l'affichage en 4 bits du numéro
    public String assembleMsgDiff(String id, String message){
        if (message.length() > TAILLEMAXMSG){
            return "";
        }

        if (numMsg == 9999){
            numMsg = 0;
        } else {
            numMsg++;
        }

        return DIFF + " " + numMsg + " " + id + " " + message + "\r\n";
    }

    public String assembleMsgEnregistrement(InetSocketAddress ip2){
        return REGI + " " + identifiant + " " + multiDiff + " " + portMultiDiff + " " + ip2 + " " + portMsg + "\r\n";
    }

    public void addToList(String msg){
        this.listMsg.add(msg);
    }
}