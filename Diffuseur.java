import java.io.*;
import java.net.*;
import java.util.*;

public class Diffuseur{
    public String identifiant;
    public DatagramSocket portMsg;
    public InetSocketAddress multiDiff;
    public DatagramSocket portMultiDiff;
    public LinkedList<String> listMsg;

    public int numMsg = 0;
    public static final String ACKM = "ACKM";
    public static final String ENDM = "ENDM";
    public static final String LAST = "LAST";
    public static final String DIFF = "DIFF";
    public static final String MESS = "MESS";
    public static final int TAILLEMAXMSG = 140;
    public static final int TAILLEID = 8;

    public Diffuseur(String id, int portRecv, int portDiff) throws Exception{
        if (portDiff < 9999 || portRecv < 9999){
            throw new Exception("[Erreur] : Le numéro du port doit être inférieur à 9999");
        }
        
        this.identifiant = id;
        this.portMsg = new DatagramSocket(portRecv);
        this.multiDiff = new InetSocketAddress(portDiff);
        this.portMultiDiff = new DatagramSocket(portDiff);
        this.listMsg = new LinkedList<String>();
    }

    //TODO: ajouter les vérifications
    public Diffuseur(String id, DatagramSocket recv, InetSocketAddress multiDiff, DatagramSocket portDiff){
        this.identifiant = id;
        this.portMsg = recv;
        this.multiDiff = multiDiff;
        this.portMultiDiff = portDiff;
        this.listMsg = new LinkedList<String>();
    }

    public String assembleMsgDiff(String id, String message){
        if (message.length() > TAILLEMAXMSG){
            return "";
        }

        if (numMsg == 9999){
            numMsg = 0;
        } else {
            numMsg++;
        }

        return DIFF + " " + numMsg + " " + id + " " + message;
    }

    public void addToList(String msg){
        this.listMsg.add(msg);
    }
}