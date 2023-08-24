import java.util.*;

public class Account {
    
    private String username;
    private String password;

    private ArrayList<String> pwds = new ArrayList<String>();
    private ArrayList<String> notes = new ArrayList<String>();
    private String pwd_decrypted;

    Account(String new_username, String new_password){
        username = new_username;
        password = new_password;
        notes = new ArrayList<String>();

    }

    public ArrayList<String> get_pwds(){

    }

    public ArrayList<String> get_notes(){
        return notes;
    }

    public String decrypt(String p){
        
    }

    public String encrypt(String p){

    }

}
