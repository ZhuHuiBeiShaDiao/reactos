/*
 *      translated by Caemyr (Jan,2008)
 *      Use ReactOS forum PM or IRC to contact me
 *      http://www.reactos.org
 *      IRC: irc.freenode.net #reactos-pl;
 */


#ifndef LANG_PL_PL_H__
#define LANG_PL_PL_H__

static MUI_ENTRY plPLLanguagePageEntries[] =
{
    {
        4,
        3,
        " ReactOS " KERNEL_VERSION_STR " Instalator ReactOS ",
        TEXT_UNDERLINE
    },
    {
        6,
        8,
        "Wyb�r j�zyka",
        TEXT_NORMAL
    },
    {
        8,
        10,
        "\x07  Prosz� wybra� j�zyk dla procesu instalacji systemu",
        TEXT_NORMAL
    },
    {
        8,
        11,
        "   i nacisn�� ENTER.",
        TEXT_NORMAL
    },
    {
        8,
        13,
        "\x07  Wybrany j�zyk b�dzie domy�lnym dla zainstalowanego systemu.",
        TEXT_NORMAL
    },
    {
        0,
        0,
        "   ENTER = Kontynuacja  F3 = Wyj�cie",
        TEXT_STATUS
    },
    {
        0,
        0,
        NULL,
        0
    }
};

static MUI_ENTRY plPLWelcomePageEntries[] =
{
    {
        4,
        3,
        " ReactOS " KERNEL_VERSION_STR " Instalator ReactOS ",
        TEXT_UNDERLINE
    },
    {
        6,
        8,
        "Witamy w programie instalacyjnym systemu ReactOS",
        TEXT_HIGHLIGHT
    },
    {
        6,
        11,
        "Na tym etapie, instalator skopiuje niezb�dne pliki systemu operacyjnego",
        TEXT_NORMAL
    },
    {
        6,
        12,
        " na tw�j komputer i przygotuje kolejny etap procesu instalacji.",
        TEXT_NORMAL
    },
    {
        8,
        15,
        "\x07  Naci�nij ENTER aby zainstalowa� system ReactOS.",
        TEXT_NORMAL
    },
    {
        8,
        17,
        "\x07  Naci�nij R aby naprawi� zainstalowany system ReactOS.",
        TEXT_NORMAL
    },
    {
        8,
        19,
        "\x07  Naci�nij L aby zapozna� si� z licencj� i warunkami korzystania z ReactOS",
        TEXT_NORMAL
    },
    {
        8,
        21,
        "\x07  Naci�nij F3 aby wyj�� bez instalacji systemu ReactOS.",
        TEXT_NORMAL
    },
    {
        6,
        23,
        "Wi�cej informacji o systemie ReactOS, mo�na znale�� na stronie:",
        TEXT_NORMAL
    },
    {
        6,
        24,
        "http://www.reactos.org",
        TEXT_HIGHLIGHT
    },
    {
        0,
        0,
        "   ENTER = Kontynuacja  R = Naprawa F3 = Wyj�cie",
        TEXT_STATUS
    },
    {
        0,
        0,
        NULL,
        0
    }
};

static MUI_ENTRY plPLIntroPageEntries[] =
{
    {
        4,
        3,
        " ReactOS " KERNEL_VERSION_STR " Instalator ReactOS ",
        TEXT_UNDERLINE
    },
    {
        6,
        8,
        "Instalator ReactOS wci�� jest we wczesnej fazie rozwoju. Nadal nie",
        TEXT_NORMAL
    },
    {
        6,
        9,
        "obs�uguje wszystkich funkcji, niezb�dnych dla programu instalacyjnego.",
        TEXT_NORMAL
    },
    {
        6,
        12,
        "Najwa�niejsze ograniczenia:",
        TEXT_NORMAL
    },
    {
        8,
        13,
        "- Instalator nie obs�uguje wi�cej ni� jednej partycji Podstawowej na dysku.",
        TEXT_NORMAL
    },
    {
        8,
        14,
        "- Instalator nie mo�e skasowa� partycji podstawowej z dysku",
        TEXT_NORMAL
    },
    {
        8,
        15,
        "  o ile nadal znajduje si� na nim tak�e partycja Rozszerzona.",
        TEXT_NORMAL
    },
    {
        8,
        16,
        "- Instalator nie mo�e skasowa� pierwszej partycji Rozszerzonej z dysku",
        TEXT_NORMAL
    },
    {
        8,
        17,
        "  o ile nadal znajduj� si� na nim kolejne partycje Rozszerzone.",
        TEXT_NORMAL
    },
    {
        8,
        18,
        "- Instalator obs�uguje jedynie system plik�w FAT.",
        TEXT_NORMAL
    },
    {
        8,
        19,
        "- Brakuje sprawdzenia poprawno�ci systemu plik�w.",
        TEXT_NORMAL
    },
    {
        8,
        23,
        "\x07  Naci�nij ENTER aby zainstalowa� system ReactOS.",
        TEXT_NORMAL
    },
    {
        8,
        25,
        "\x07  Naci�nij F3 aby wyj�� bez instalacji systemu ReactOS.",
        TEXT_NORMAL
    },
    {
        0,
        0,
        "   ENTER = Kontynuacja   F3 = Wyj�cie",
        TEXT_STATUS
    },
    {
        0,
        0,
        NULL,
        0
    }
};

static MUI_ENTRY plPLLicensePageEntries[] =
{
    {
        4,
        3,
        " ReactOS " KERNEL_VERSION_STR " Instalator ReactOS ",
        TEXT_UNDERLINE
    },
    {
        6,
        6,
        "Licencja:",
        TEXT_HIGHLIGHT
    },
    {
        8,
        8,
        "System ReactOS jest licencjonowany na warunkach licencji",
        TEXT_NORMAL
    },
    {
        8,
        9,
        "GNU GPL z elementami kodu pochodz�cego z kompatybilnych",
        TEXT_NORMAL
    },
    {
        8,
        10,
        "licencji, takich jak X11 czy BSD albo GNU LGPL. Ca�e",
        TEXT_NORMAL
    },
    {
        8,
        11,
        "oprogramowanie, b�d�ce cz�sci� system ReactOS podlega wi�c",
        TEXT_NORMAL
    },
    {
        8,
        12,
        "licencji GNU GPL jak i odpowiedniej licencji oryginalnej.",
        TEXT_NORMAL
    },
    {
        8,
        13,
        "To oprogramowanie wydawane jest BEZ JAKIEJKOLWIEK gwarancji",
        TEXT_NORMAL
    },
    {
        8,
        15,
        "czy te� ogranicze� w u�yciu, poza przepisami prawa lokalnego",
        TEXT_NORMAL
    },
    {
        8,
        16,
        "b�d� mi�dzynarodowego. Licencja systemu ReactOS",
        TEXT_NORMAL
    },
    {
        8,
        17,
        "reguluje wy��cznie zasady dystrybucji dla os�b trzecich.",
        TEXT_NORMAL
    },
    {
        8,
        18,
        "Je�li z jakiego� powodu nie otrzyma�e� kopii licencji",
        TEXT_NORMAL
    },
    {
        8,
        19,
        "GNU GPL wraz z systemem ReactOS, prosimy odwiedzi� stron�:",
        TEXT_NORMAL
    },
    {
        8,
        20,
        "http://www.gnu.org/licenses/licenses.html",
        TEXT_HIGHLIGHT
    },
    {
        8,
        22,
        "Gwarancja:",
        TEXT_HIGHLIGHT
    },
    {
        8,
        24,
        "Niniejszy program jest wolnym oprogramowaniem; szczeg�y",
        TEXT_NORMAL
    },
    {
        8,
        25,
        "dotycz�ce kopiowania w �r�d�ach. Nie ma �ADNEJ gwarancji",
        TEXT_NORMAL
    },
    {
        8,
        26,
        "PRZYDATNO�CI HANDLOWEJ LUB DO OKRE�LONYCH ZASTOSOWA�",
        TEXT_NORMAL
    },
    {
        0,
        0,
        "   ENTER = Powr�t",
        TEXT_STATUS
    },
    {
        0,
        0,
        NULL,
        0
    }
};

static MUI_ENTRY plPLDevicePageEntries[] =
{
    {
        4,
        3,
        " ReactOS " KERNEL_VERSION_STR " Instalator ReactOS ",
        TEXT_UNDERLINE
    },
    {
        6,
        8,
        "Poni�sza lista zawiera obecne ustawienia sprz�tu.",
        TEXT_NORMAL
    },
    {
        8,
        11,
        "       Komputer:",
        TEXT_NORMAL
    },
    {
        8,
        12,
        "        Ekran:",
        TEXT_NORMAL,
    },
    {
        8,
        13,
        "       Klawiatura:",
        TEXT_NORMAL
    },
    {
        8,
        14,
        "Uk�ad klawiatury:",
        TEXT_NORMAL
    },
    {
        8,
        16,
        "         Zaakceptuj:",
        TEXT_NORMAL
    },
    {
        25,
        16, "Zaakceptuj te ustawienia sprz�tu",
        TEXT_NORMAL
    },
    {
        6,
        19,
        "Mo�esz zmieni� poszczeg�lne ustawienia za pomoc� klawiszy G�RA i Dӣ",
        TEXT_NORMAL
    },
    {
        6,
        20,
        "aby wybra� kategori�. Potem naci�nij ENTER by przej�� do menu z ",
        TEXT_NORMAL
    },
    {
        6,
        21,
        "ustawieniami szczeg�owymi do wyboru.",
        TEXT_NORMAL
    },
    {
        6,
        23,
        "Kiedy wszystkie ustawienia s� poprawne, wybierz \"Zaakceptuj te ustawienia sprz�tu\"",
        TEXT_NORMAL
    },
    {
        6,
        24,
        "i naci�nij ENTER.",
        TEXT_NORMAL
    },
    {
        0,
        0,
        "   ENTER = Kontynuacja   F3 = Wyj�cie",
        TEXT_STATUS
    },
    {
        0,
        0,
        NULL,
        0
    }
};

static MUI_ENTRY plPLRepairPageEntries[] =
{
    {
        4,
        3,
        " ReactOS " KERNEL_VERSION_STR " Instalator ReactOS ",
        TEXT_UNDERLINE
    },
    {
        6,
        8,
        "Instalator ReactOS wci�� jest we wczesnej fazie rozwoju. Nadal nie",
        TEXT_NORMAL
    },
    {
        6,
        9,
        "obs�uguje wszystkich funkcji, niezb�dnych dla programu instalacyjnego.",
        TEXT_NORMAL
    },
    {
        6,
        12,
        "Naprawa istniej�cej instalacji systemu nie jest jeszcze mo�liwa.",
        TEXT_NORMAL
    },
    {
        8,
        15,
        "\x07  Naci�nij U �eby uaktualni� system.",
        TEXT_NORMAL
    },
    {
        8,
        17,
        "\x07  Naci�nij R by uruchomi� Konsol� Odtwarzania.",
        TEXT_NORMAL
    },
    {
        8,
        19,
        "\x07  Naci�nij ESC by powr�ci� do g��wnego menu.",
        TEXT_NORMAL
    },
    {
        8,
        21,
        "\x07  Naci�nij ENTER by zrestartowa� komputer.",
        TEXT_NORMAL
    },
    {
        0,
        0,
        "   ESC = Menu g��wne  ENTER = Restart",
        TEXT_STATUS
    },
    {
        0,
        0,
        NULL,
        0
    }
};
static MUI_ENTRY plPLComputerPageEntries[] =
{
    {
        4,
        3,
        " ReactOS " KERNEL_VERSION_STR " Instalator ReactOS ",
        TEXT_UNDERLINE
    },
    {
        6,
        8,
        "Aby zmieni� typ komputera, na kt�rym chcesz zainstalowa�",
        TEXT_NORMAL
    },
    {
        8,
        10,
        "\x07  naci�nij klawisz G�RA albo Dӣ by wybra� odpowiedni typ komputera.",
        TEXT_NORMAL
    },
    {
        8,
        11,
        "   Nast�pnie naci�nij ENTER.",
        TEXT_NORMAL
    },
    {
        8,
        13,
        "\x07  Naci�nij ESC key aby powr�ci� do poprzedniej strony bez zmiany",
        TEXT_NORMAL
    },
    {
        8,
        14,
        "   typu komputera.",
        TEXT_NORMAL
    },
    {
        0,
        0,
        "   ENTER = Kontynuacja   ESC = Anulowanie   F3 = Wyj�cie",
        TEXT_STATUS
    },
    {
        0,
        0,
        NULL,
        0
    }
};

static MUI_ENTRY plPLFlushPageEntries[] =
{
    {
        4,
        3,
        " ReactOS " KERNEL_VERSION_STR " Instalator ReactOS ",
        TEXT_UNDERLINE
    },
    {
        10,
        6,
        "System obecnie sprawdza czy dane s� poprawnie zapisane na dysku",
        TEXT_NORMAL
    },
    {
        10,
        8,
        "To mo�e zaj�� minut�.",
        TEXT_NORMAL
    },
    {
        10,
        9,
        "Po zako�czeniu, system zrestartuje komputer automatycznie.",
        TEXT_NORMAL
    },
    {
        0,
        0,
        "   Czyszczenie pamieci Cache",
        TEXT_STATUS
    },
    {
        0,
        0,
        NULL,
        0
    }
};

static MUI_ENTRY plPLQuitPageEntries[] =
{
    {
        4,
        3,
        " ReactOS " KERNEL_VERSION_STR " Instalator ReactOS ",
        TEXT_UNDERLINE
    },
    {
        10,
        6,
        "ReactOS zosta� ju� poprawnie zainstalowany",
        TEXT_NORMAL
    },
    {
        10,
        8,
        "Upewnij si� ze w Nap�dzie A: nie ma dyskietki i",
        TEXT_NORMAL
    },
    {
        10,
        9,
        "w nap�dach optycznych - �adnych CD-ROM�w.",
        TEXT_NORMAL
    },
    {
        10,
        11,
        "Naci�nij ENTER by zrestartowa� komputer.",
        TEXT_NORMAL
    },
    {
        0,
        0,
        "   Prosz� czeka� ...",
        TEXT_STATUS,
    },
    {
        0,
        0,
        NULL,
        0
    }
};

static MUI_ENTRY plPLDisplayPageEntries[] =
{
    {
        4,
        3,
        " ReactOS " KERNEL_VERSION_STR " Instalator ReactOS ",
        TEXT_UNDERLINE
    },
    {
        6,
        8,
        "Chcesz zmieni� rozdzielczo�� ekranu.",
        TEXT_NORMAL
    },
    {   8,
        10,
         "\x07  Naci�nij klawisz G�RA albo Dӣ by wybra� odpowiedni typ komputera.",
         TEXT_NORMAL
    },
    {
        8,
        11,
        "   Nast�pnie naci�nij ENTER.",
        TEXT_NORMAL
    },
    {
        8,
        13,
        "\x07  Naci�nij ESC key aby powr�ci� do poprzedniej strony bez zmiany",
        TEXT_NORMAL
    },
    {
        8,
        14,
        "   rodzaju ekranu.",
        TEXT_NORMAL
    },
    {
        0,
        0,
        "   ENTER = Kontynuacja   ESC = Anulowanie   F3 = Wyj�cie",
        TEXT_STATUS
    },
    {
        0,
        0,
        NULL,
        0
    }
};

static MUI_ENTRY plPLSuccessPageEntries[] =
{
    {
        4,
        3,
        " ReactOS " KERNEL_VERSION_STR " Instalator ReactOS ",
        TEXT_UNDERLINE
    },
    {
        10,
        6,
        "Podstawowe sk�adniki systemu ReactOS zosta�y zainstalowane.",
        TEXT_NORMAL
    },
    {
        10,
        8,
        "Upewnij si� ze w Nap�dzie A: nie ma dyskietki i",
        TEXT_NORMAL
    },
    {
        10,
        9,
        "w nap�dach optycznych - �adnych CD-ROM�w.",
        TEXT_NORMAL
    },
    {
        10,
        11,
        "Naci�nij ENTER by zrestartowa� komputer.",
        TEXT_NORMAL
    },
    {
        0,
        0,
        "   ENTER = Restart komputera",
        TEXT_STATUS
    },
    {
        0,
        0,
        NULL,
        0
    }
};

static MUI_ENTRY plPLBootPageEntries[] =
{
    {
        4,
        3,
        " ReactOS " KERNEL_VERSION_STR " Instalator ReactOS ",
        TEXT_UNDERLINE
    },
    {
        6,
        8,
        "Instalator ReactOS nie mo�e wgra� bootloadera na tw�j",
        TEXT_NORMAL
    },
    {
        6,
        9,
        "dysk twardy",
        TEXT_NORMAL
    },
    {
        6,
        13,
        "Prosz� umie�ci� sformatowan� dyskietk� w nap�dzie A:",
        TEXT_NORMAL
    },
    {
        6,
        14,
        "i nacisn�� klawisz ENTER.",
        TEXT_NORMAL,
    },
    {
        0,
        0,
        "   ENTER = Kontynuacja   F3 = Wyj�cie",
        TEXT_STATUS
    },
    {
        0,
        0,
        NULL,
        0
    }

};

static MUI_ENTRY plPLSelectPartitionEntries[] =
{
    {
        4,
        3,
        " ReactOS " KERNEL_VERSION_STR " Instalator ReactOS ",
        TEXT_UNDERLINE
    },
    {
        6,
        8,
        "Poni�sza lista pokazuje istniej�ce partycje i puste",
        TEXT_NORMAL
    },
    {
        6,
        9,
        "miejsce na nowe partycje.",
        TEXT_NORMAL
    },
    {
        8,
        11,
        "\x07  Naci�nij G�RA lub Dӣ by wybra� pozycj� z listy.",
        TEXT_NORMAL
    },
    {
        8,
        13,
        "\x07  Naci�nij ENTER by zainstalowa� ReactOS na wybranej partycji.",
        TEXT_NORMAL
    },
    {
        8,
        15,
        "\x07  Naci�nij C by stworzy� now� partycj�.",
        TEXT_NORMAL
    },
    {
        8,
        17,
        "\x07  Naci�nij D by skasowa� istniej�c� partycj�.",
        TEXT_NORMAL
    },
    {
        0,
        0,
        "   Prosz� czeka�...",
        TEXT_STATUS
    },
    {
        0,
        0,
        NULL,
        0
    }
};

static MUI_ENTRY plPLFormatPartitionEntries[] =
{
    {
        4,
        3,
        " ReactOS " KERNEL_VERSION_STR " Instalator ReactOS ",
        TEXT_UNDERLINE
    },
    {
        6,
        8,
        "Formatowanie partycji",
        TEXT_NORMAL
    },
    {
        6,
        10,
        "Instalator sformatuje teraz partycj�. Naci�nij ENTER aby kontynuowa�.",
        TEXT_NORMAL
    },
    {
        0,
        0,
        "   ENTER = Kontynuacja   F3 = Wyj�cie",
        TEXT_STATUS
    },
    {
        0,
        0,
        NULL,
        TEXT_NORMAL
    }
};

static MUI_ENTRY plPLInstallDirectoryEntries[] =
{
    {
        4,
        3,
        " ReactOS " KERNEL_VERSION_STR " Instalator ReactOS ",
        TEXT_UNDERLINE
    },
    {
        6,
        8,
        "Instalator przegra pliki systemu na wybran� partycj�. Wybierz",
        TEXT_NORMAL
    },
    {
        6,
        9,
        "katalog do kt�rego chcesz zainstalowa� system ReactOS:",
        TEXT_NORMAL
    },
    {
        6,
        14,
        "Aby zmieni� domy�lny katalog, u�yj klawisza BACKSPACE by skasowa�",
        TEXT_NORMAL
    },
    {
        6,
        15,
        "a nast�pnie wprowad� now� �cie�k� do katalogu, do kt�rego system",
        TEXT_NORMAL
    },
    {
        6,
        16,
        "ma zosta� zainstalowany.",
        TEXT_NORMAL
    },
    {
        0,
        0,
        "   ENTER = Kontynuacja   F3 = Wyj�cie",
        TEXT_STATUS
    },
    {
        0,
        0,
        NULL,
        0
    }
};

static MUI_ENTRY plPLFileCopyEntries[] =
{
    {
        4,
        3,
        " ReactOS " KERNEL_VERSION_STR " Instalator ReactOS ",
        TEXT_UNDERLINE
    },
    {
        11,
        12,
        "Prosz� czeka�, Instalator ReactOS kopiuje pliki do wybranego",
        TEXT_NORMAL
    },
    {
        30,
        13,
        "katalogu.",
        TEXT_NORMAL
    },
    {
        20,
        14,
        "To mo�e zaj�� kilka minut.",
        TEXT_NORMAL
    },
    {
        0,
        0,
        "                                                           \xB3 Prosz� czeka�...    ",
        TEXT_STATUS
    },
    {
        0,
        0,
        NULL,
        0
    }
};

static MUI_ENTRY plPLBootLoaderEntries[] =
{
    {
        4,
        3,
        " ReactOS " KERNEL_VERSION_STR " Instalator ReactOS ",
        TEXT_UNDERLINE
    },
    {
        6,
        8,
        "Instalator musi teraz wgra� bootloader",
        TEXT_NORMAL
    },
    {
        8,
        12,
        "Wgraj bootloader na dysk twardy (MBR).",
        TEXT_NORMAL
    },
    {
        8,
        13,
        "Wgraj bootloader na dyskietk�.",
        TEXT_NORMAL
    },
    {
        8,
        14,
        "Pomi� wgrywanie bootloadera.",
        TEXT_NORMAL
    },
    {
        0,
        0,
        "   ENTER = Kontynuacja   F3 = Wyj�cie",
        TEXT_STATUS
    },
    {
        0,
        0,
        NULL,
        0
    }
};

static MUI_ENTRY plPLKeyboardSettingsEntries[] =
{
    {
        4,
        3,
        " ReactOS " KERNEL_VERSION_STR " Instalator ReactOS ",
        TEXT_UNDERLINE
    },
    {
        6,
        8,
        "Chcesz zmieni� typ klawiatury, jaki ma by� zainstalowany.",
        TEXT_NORMAL
    },
    {
        8,
        10,
        "\x07  Naci�nij klawisz G�RA albo Dӣ by wybra� odpowiedni typ klawiatury.",
        TEXT_NORMAL
    },
    {
        8,
        11,
        "   Nast�pnie naci�nij ENTER.",
        TEXT_NORMAL
    },
    {
        8,
        13,
        "\x07  Naci�nij ESC key aby powr�ci� do poprzedniej strony bez zmiany",
        TEXT_NORMAL
    },
    {
        8,
        14,
        "   typu klawiatury.",
        TEXT_NORMAL
    },
    {
        0,
        0,
        "   ENTER = Kontynuacja   ESC = Anulowanie   F3 = Wyj�cie",
        TEXT_STATUS
    },
    {
        0,
        0,
        NULL,
        0
    }
};

static MUI_ENTRY plPLLayoutSettingsEntries[] =
{
    {
        4,
        3,
        " ReactOS " KERNEL_VERSION_STR " Instalator ReactOS ",
        TEXT_UNDERLINE
    },
    {
        6,
        8,
        "Chcesz zmieni� uk�ad klawiatury, jaki ma by� zainstalowany.",
        TEXT_NORMAL
    },
    {
        8,
        10,
        "\x07  Naci�nij klawisz G�RA albo Dӣ by wybra� odpowiedni uk�ad",
        TEXT_NORMAL
    },
    {
        8,
        11,
        "    klawiatury. Nast�pnie naci�nij ENTER.",
        TEXT_NORMAL
    },
    {
        8,
        13,
        "\x07  Naci�nij ESC key aby powr�ci� do poprzedniej strony bez zmiany",
        TEXT_NORMAL
    },
    {
        8,
        14,
        "   uk�adu klawiatury.",
        TEXT_NORMAL
    },
    {
        0,
        0,
        "   ENTER = Kontynuacja   ESC = Anulowanie   F3 = Wyj�cie",
        TEXT_STATUS
    },
    {
        0,
        0,
        NULL,
        0
    },

};

static MUI_ENTRY plPLPrepareCopyEntries[] =
{
    {
        4,
        3,
        " ReactOS " KERNEL_VERSION_STR " Instalator ReactOS ",
        TEXT_UNDERLINE
    },
    {
        6,
        8,
        "Instalator przygotuje tw�j komputer do skopiowania plik�w systemu. ",
        TEXT_NORMAL
    },
    {
        0,
        0,
        "   Tworzenie listy plik�w do skopiowania...",
        TEXT_STATUS
    },
    {
        0,
        0,
        NULL,
        0
    },

};

static MUI_ENTRY plPLSelectFSEntries[] =
{
    {
        4,
        3,
        " ReactOS " KERNEL_VERSION_STR " Instalator ReactOS ",
        TEXT_UNDERLINE
    },
    {
        6,
        17,
        "Wybierz system plik�w z listy poni�ej.",
        0
    },
    {
        8,
        19,
        "\x07  Naci�nij klawisz G�Ra alub Dӣ by wybra� system plik�w.",
        0
    },
    {
        8,
        21,
        "\x07  Naci�nij ENTER aby sformatowa� partycj�.",
        0
    },
    {
        8,
        23,
        "\x07  Naci�nij ESC aby wybra� inn� partycj�.",
        0
    },
    {
        0,
        0,
        "   ENTER = Kontynuacja   ESC = Anulowanie   F3 = Wyj�cie",
        TEXT_STATUS
    },

    {
        0,
        0,
        NULL,
        0
    }
};

static MUI_ENTRY plPLDeletePartitionEntries[] =
{
    {
        4,
        3,
        " ReactOS " KERNEL_VERSION_STR " Instalator ReactOS ",
        TEXT_UNDERLINE
    },
    {
        6,
        8,
        "Chcesz skasowa� wybran� partycj�",
        TEXT_NORMAL
    },
    {
        8,
        18,
        "\x07  Naci�nij D by skasowa� partycj�.",
        TEXT_NORMAL
    },
    {
        11,
        19,
        "UWAGA: Wszystkie dane, zapisane na tej partycji zostan� skasowane!",
        TEXT_NORMAL
    },
    {
        8,
        21,
        "\x07  Naci�nij ESC aby anulowa�.",
        TEXT_NORMAL
    },
    {
        0,
        0,
        "   D = Skasowanie partycji   ESC = Anulowanie   F3 = Wyj�cie",
        TEXT_STATUS
    },
    {
        0,
        0,
        NULL,
        0
    }
};

static MUI_ENTRY plPLRegistryEntries[] =
{
    {
        4,
        3,
        " ReactOS " KERNEL_VERSION_STR " Instalator ReactOS ",
        TEXT_UNDERLINE
    },
    {
        6,
        8,
        "Instalator uaktualnia w�a�nie konfiguracj� systemu. ",
        TEXT_NORMAL
    },
    {
        0,
        0,
        "   Tworzenie ga��zi rejestru...",
        TEXT_STATUS
    },
    {
        0,
        0,
        NULL,
        0
    },

};

MUI_ERROR plPLErrorEntries[] =
{
    {
        //ERROR_NOT_INSTALLED
        "ReactOS nie zosta� w pe�ni zainstalowany na twoim\n"
	     "komputerze. Je�li wyjdziesz teraz, trzeba b�dzie\n"
	     "ponownie uruchomi� instalator by zainstalowa� ReactOS.\n"
	     "\n"
	     "  \x07  Naci�nij ENTER aby kontynuowa� instalacj�.\n"
	     "  \x07  Naci�nij F3 aby wyj�� z instalatora.",
	     "F3= Quit  ENTER = Continue"
    },
    {
        //ERROR_NO_HDD
        "Instalator nie znalaz� �adnego dysku twardego.\n",
        "ENTER = Restart komputera"
    },
    {
        //ERROR_NO_SOURCE_DRIVE
        "Instalator nie znalaz� nap�du �r�d�owego.\n",
        "ENTER = Restart komputera"
    },
    {
        //ERROR_LOAD_TXTSETUPSIF
        "Instalator nie m�g� za�adowa� pliku TXTSETUP.SIF.\n",
        "ENTER = Restart komputera"
    },
    {
        //ERROR_CORRUPT_TXTSETUPSIF
        "Instalator znalaz� uszkodzony plik TXTSETUP.SIF.\n",
        "ENTER = Restart komputera"
    },
    {
        //ERROR_SIGNATURE_TXTSETUPSIF,
        "Instalator znalaz� nieprawid�owy wpis w TXTSETUP.SIF.\n",
        "ENTER = Restart komputera"
    },
    {
        //ERROR_DRIVE_INFORMATION
        "Instalator nie m�g� odczyta� informacji o nap�dzie.\n",
        "ENTER = Restart komputera"
    },
    {
        //ERROR_WRITE_BOOT,
        "Nieudane zapisanie FAT bootcode na partycji systemowej.",
        "ENTER = Restart komputera"
    },
    {
        //ERROR_LOAD_COMPUTER,
        "Instalator failed to load the computer type list.\n",
        "ENTER = Restart komputera"
    },
    {
        //ERROR_LOAD_DISPLAY,
        "Instalator nie m�g� za�adowa� listy ustawie� ekranu.\n",
        "ENTER = Restart komputera"
    },
    {
        //ERROR_LOAD_KEYBOARD,
        "Instalator nie m�g� za�adowa� listy typ�w klawiatury.\n",
        "ENTER = Restart komputera"
    },
    {
        //ERROR_LOAD_KBLAYOUT,
        "Instalator nie m�g� za�adowa� listy uk�ad�w klawiatury.\n",
        "ENTER = Restart komputera"
    },
    {
        //ERROR_WARN_PARTITION,
          "Instalator wykry�, �e co najmniej jeden dysk twardy zawiera niekompatybiln� \n"
		  "tablic� partycji, kt�ra nie b�dzie poprawnie obs�ugiwana!\n"
		  "\n"
		  "Tworzenie lub kasowanie partycji mo�e zniszczy� ca�� tablic� partycji.\n"
		  "\n"
		  "  \x07  Naci�nij F3 aby wyj�� z instalatora."
		  "  \x07  Naci�nij ENTER aby kontynuowa�.",
          "F3= Wyj�cie  ENTER = Kontynuacja"
    },
    {
        //ERROR_NEW_PARTITION,
        "Nie mo�esz stworzy� nowej partycji w miejscu ju�\n"
		"istniej�cej!\n"
		"\n"
		"  * Naci�nij dowolny klawisz aby kontynuowa�.",
        NULL
    },
    {
        //ERROR_DELETE_SPACE,
        "Nie mo�esz skasowa� pustej przestrzeni, gdzie nie ma �adnej partycji!\n"
        "\n"
        "  * Naci�nij dowolny klawisz aby kontynuowa�.",
        NULL
    },
    {
        //ERROR_INSTALL_BOOTCODE,
        "Nieudana instalacja FAT bootcode na partycji systemowej.",
        "ENTER = Restart komputera"
    },
    {
        //ERROR_NO_FLOPPY,
        "Brak dyskietki w nap�dzie A:.",
        "ENTER = Kontynuacja"
    },
    {
        //ERROR_UPDATE_KBSETTINGS,
        "Instalator nie m�g� zmieni� uk�adu klawiatury.",
        "ENTER = Restart komputera"
    },
    {
        //ERROR_UPDATE_DISPLAY_SETTINGS,
        "Instalator nie m�g� zmieni� ustawie� ekranu w rejestrze.",
        "ENTER = Restart komputera"
    },
    {
        //ERROR_IMPORT_HIVE,
        "Instalator nie by� w stanie zaimportowa� pliku ga��zi rejestru.",
        "ENTER = Restart komputera"
    },
    {
        //ERROR_FIND_REGISTRY
        "Instalator nie by� w stanie znale�� plik�w z danymi rejestru.",
        "ENTER = Restart komputera"
    },
    {
        //ERROR_CREATE_HIVE,
        "Instalator nie m�g� stworzy� ga��zi rejestru.",
        "ENTER = Restart komputera"
    },
    {
        //ERROR_INITIALIZE_REGISTRY,
        "Instalator nie by� w stanie ustawic inicjalizacji rejestru.",
        "ENTER = Restart komputera"
    },
    {
        //ERROR_INVALID_CABINET_INF,
        "Cabinet nie zawiera prawid�owego pliku inf.\n",
        "ENTER = Restart komputera"
    },
    {
        //ERROR_CABINET_MISSING,
        "Cabinet nie zosta� znaleziony.\n",
        "ENTER = Restart komputera"
    },
    {
        //ERROR_CABINET_SCRIPT,
        "Cabinet nie zawiera skryptu instalacji.\n",
        "ENTER = Restart komputera"
    },
    {
        //ERROR_COPY_QUEUE,
        "Instalator nie by� w stanie otworzy� kolejki kopiowania pliku.\n",
        "ENTER = Restart komputera"
    },
    {
        //ERROR_CREATE_DIR,
        "Instalator nie m�g� stworzy� katalog�w.",
        "ENTER = Restart komputera"
    },
    {
        //ERROR_TXTSETUP_SECTION,
        "Instalator nie by� w stanie znale�� sekcji 'Directories'\n"
        "w pliku TXTSETUP.SIF.\n",
        "ENTER = Restart komputera"
    },
    {
        //ERROR_CABINET_SECTION,
        "Instalator nie by� w stanie znale�� sekcji 'Directories'\n"
        "w pliku cabinet.\n",
        "ENTER = Restart komputera"
    },
    {
        //ERROR_CREATE_INSTALL_DIR
        "Instalator nie m�g� stworzy� katalogu instalacji.",
        "ENTER = Restart komputera"
    },
    {
        //ERROR_FIND_SETUPDATA,
        "Instalator nie by� w stanie znale�� sekcji 'SetupData'\n"
		 "w pliku TXTSETUP.SIF.\n",
		 "ENTER = Restart komputera"
    },
    {
        //ERROR_WRITE_PTABLE,
        "Instalator nie m�g� zapisa� zmian w tablicy partycji.\n"
        "ENTER = Restart komputera"
    },
    {
        NULL,
        NULL
    }
};


MUI_PAGE plPLPages[] =
{
    {
        LANGUAGE_PAGE,
        plPLLanguagePageEntries
    },
    {
        START_PAGE,
        plPLWelcomePageEntries
    },
    {
        INSTALL_INTRO_PAGE,
        plPLIntroPageEntries
    },
    {
        LICENSE_PAGE,
        plPLLicensePageEntries
    },
    {
        DEVICE_SETTINGS_PAGE,
        plPLDevicePageEntries
    },
    {
        REPAIR_INTRO_PAGE,
        plPLRepairPageEntries
    },
    {
        COMPUTER_SETTINGS_PAGE,
        plPLComputerPageEntries
    },
    {
        DISPLAY_SETTINGS_PAGE,
        plPLDisplayPageEntries
    },
    {
        FLUSH_PAGE,
        plPLFlushPageEntries
    },
    {
        SELECT_PARTITION_PAGE,
        plPLSelectPartitionEntries
    },
    {
        SELECT_FILE_SYSTEM_PAGE,
        plPLSelectFSEntries
    },
    {
        FORMAT_PARTITION_PAGE,
        plPLFormatPartitionEntries
    },
    {
        DELETE_PARTITION_PAGE,
        plPLDeletePartitionEntries
    },
    {
        INSTALL_DIRECTORY_PAGE,
        plPLInstallDirectoryEntries
    },
    {
        PREPARE_COPY_PAGE,
        plPLPrepareCopyEntries
    },
    {
        FILE_COPY_PAGE,
        plPLFileCopyEntries
    },
    {
        KEYBOARD_SETTINGS_PAGE,
        plPLKeyboardSettingsEntries
    },
    {
        BOOT_LOADER_PAGE,
        plPLBootLoaderEntries
    },
    {
        LAYOUT_SETTINGS_PAGE,
        plPLLayoutSettingsEntries
    },
    {
        QUIT_PAGE,
        plPLQuitPageEntries
    },
    {
        SUCCESS_PAGE,
        plPLSuccessPageEntries
    },
    {
        BOOT_LOADER_FLOPPY_PAGE,
        plPLBootPageEntries
    },
    {
        REGISTRY_PAGE,
        plPLRegistryEntries
    },
    {
        -1,
        NULL
    }
};

#endif
