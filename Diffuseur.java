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
    public static final int TAILLEMAXMSG = 140;
    public static final int TAILLEID = 8;

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