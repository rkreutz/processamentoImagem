#include "stdafx.h"
#include "language.h"
#include "utils.h"
#include "license.h"

void initialize_table(std::string lan) {
	if (lan == "BR") {
		texts[E_TEXT::trademark] = "A Licensa deste programa segue.";
		texts[E_TEXT::ending] = "Final do programa.";
		texts[E_TEXT::license_error1] = "Arquitetura do sistema operacional nao e valida, por favor especifique outro tipo de arquitetura.";
	}

	else if (lan == "EN") {
		texts[E_TEXT::trademark] = "The license of this program is as follows.";
		texts[E_TEXT::ending] = "End of the pragram.";
		texts[E_TEXT::license_error1] = "Please specify a valid OS architecture.";
		texts[E_TEXT::license_error2] = "";
	}

	else {
		std::cout << "\nLanguage not supported, please change it to BR or EN.\n";
		exit(-2);
	}
}