import java.util.Scanner;

public class analisisNumerico {

    // Declaraciones nativas para los métodos de C
    public native double puntoFijo(double x0, double tol, int maxIter);
    public native double biseccion(double a, double b, double tol, int maxIter);
    public native double newtonRaphson(double x0, double tol, int maxIter);

    static {
        // Cargar la librería nativa
        System.loadLibrary("analisisnumerico");
    }

    // Método principal con menú
    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);
        analisisNumerico analisis = new analisisNumerico();
        
        while (true) {
            // Menú de opciones
            System.out.println("\nElija el método que desea utilizar:");
            System.out.println("1. Punto Fijo");
            System.out.println("2. Bisección");
            System.out.println("3. Newton-Raphson");
            System.out.println("4. Salir");
            System.out.print("Opción: ");
            
            int opcion = scanner.nextInt();
            
            if (opcion == 4) {
                System.out.println("¡Hasta luego!");
                break;
            }

            // Solicitar datos según la opción elegida
            try {
                double resultado = 0;
                switch (opcion) {
                    case 1:
                        System.out.print("Ingrese el valor inicial (x0): ");
                        double x0 = scanner.nextDouble();
                        System.out.print("Ingrese la tolerancia: ");
                        double tol1 = scanner.nextDouble();
                        System.out.print("Ingrese el número máximo de iteraciones: ");
                        int maxIter1 = scanner.nextInt();
                        resultado = analisis.puntoFijo(x0, tol1, maxIter1);
                        System.out.println("Resultado del Punto Fijo: " + resultado);
                        break;
                    case 2:
                        System.out.print("Ingrese el valor de a: ");
                        double a = scanner.nextDouble();
                        System.out.print("Ingrese el valor de b: ");
                        double b = scanner.nextDouble();
                        System.out.print("Ingrese la tolerancia: ");
                        double tol2 = scanner.nextDouble();
                        System.out.print("Ingrese el número máximo de iteraciones: ");
                        int maxIter2 = scanner.nextInt();
                        resultado = analisis.biseccion(a, b, tol2, maxIter2);
                        System.out.println("Resultado de la Bisección: " + resultado);
                        break;
                    case 3:
                        System.out.print("Ingrese el valor inicial (x0): ");
                        double x0Newton = scanner.nextDouble();
                        System.out.print("Ingrese la tolerancia: ");
                        double tol3 = scanner.nextDouble();
                        System.out.print("Ingrese el número máximo de iteraciones: ");
                        int maxIter3 = scanner.nextInt();
                        resultado = analisis.newtonRaphson(x0Newton, tol3, maxIter3);
                        System.out.println("Resultado de Newton-Raphson: " + resultado);
                        break;
                    default:
                        System.out.println("Opción inválida. Intente de nuevo.");
                        break;
                }
            } catch (Exception e) {
                // Capturamos la excepción lanzada desde C y mostramos el mensaje
                System.out.println("Error: " + e.getMessage());
            }
        }

        scanner.close();
    }
}

