import java.net.*;
import java.io.*;
import java.lang.*;

public class EcouteUtilisateur implements Runnable{
    public Socket socket;
    //FIXME: message stocké sommairement ici à revoir
    public String msgUtilisateur;

    public EcouteUtilisateur(Socket s){
        this.socket = s;
        this.msgUtilisateur = "";
    }

    public void run(){
        try{
            while(true){
                BufferedReader br = new BufferedReader(new InputStreamReader(socket.getInputStream()));
                PrintWriter pw = new PrintWriter(new OutputStreamWriter(socket.getOutputStream()));

                String msg = br.readLine();
                String [] traitement = msg.split(" ");

                if (traitement[0].equals(Diffuseur.MESS) 
                    && traitement[1].length() <= 8 
                    && traitement[2].length() <= 140) {
                    this.msgUtilisateur = traitement[2];

                    pw.print(Diffuseur.ACKM + "\n");
                    pw.flush();

                    this.socket.close();
                    break;
                } else if (traitement[0].equals(Diffuseur.LAST)) {
                    try{
                        int nbMsg = Integer.valueOf(traitement[1]);
                        //TODO: implémenter l'envoi des messages
                        pw.print(Diffuseur.ENDM + "\n");
                        pw.flush();
                    } catch(NumberFormatException e) {
                        pw.print("[Erreur] : Format de nombre invalide\n");
                        pw.flush();
                    }
                } else {
                    pw.print("[Erreur] : Message au mauvais format\n");
                    pw.flush();
                }
            }
        } catch(Exception e) {
            System.out.println(e);
            e.printStackTrace();
        }
    }
}
