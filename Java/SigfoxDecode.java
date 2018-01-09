/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package javatests;

import java.math.BigInteger;

/**
 *
 * @author frup43821
 */
public class SigfoxDecoder {

    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        // TODO code application logic here
        String hexVal = "1c";

        String char1 = new BigInteger(String.valueOf(hexVal.charAt(0)), 16).toString(2);
        while (char1.length() < 4) {
            char1 = "0" + char1;
        }
        String char2 = new BigInteger(String.valueOf(hexVal.charAt(1)), 16).toString(2);
        while (char2.length() < 4) {
            char2 = "0" + char2;
        }

        System.out.println(char1 + char2);

        int value = 0;

        for (int i = char1.length() - 1; i >= 0; i--) {
            if (char1.charAt(i) == '1') {
                value |= 1 << 4 + (3 - i);
            }
        }
        for (int i = char2.length() - 1; i >= 0; i--) {
            if (char2.charAt(i) == '1') {
                value |= 1 << (3 - i);
            }
        }

        System.out.println(value + "%");
    }

}
