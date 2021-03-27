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
    public final String ackm = "ACKM";
    public final String endm = "ENDM";
    public final String last = "LAST";
    public final int tailleMaxMsg = 140;
    public final int tailleId = 8;

    //TODO: ajouter les vérifications
    public Diffuseur(String id, DatagramSocket recv, InetSocketAddress multiDiff, DatagramSocket portDiff){
        this.identifiant = id;
        this.portMsg = recv;
        this.multiDiff = multiDiff;
        this.portMultiDiff = portDiff;
        this.listMsg = new LinkedList<String>();
    }

    public String assembleMsgDiff(String id, String message){
        if (message.length() > tailleMaxMsg){
            return "";
        }

        if (numMsg == 9999){
            numMsg = 0;
        } else {
            numMsg++;
        }

        return "Diff " + numMsg + " " + id + " " + message;
    }

    public void addToList(String msg){
        this.listMsg.add(msg);
    }
}