#include <iostream>
#include <string>
#include <cstring>
using namespace std;

// === Struct Film (Binary Search Tree) ===
struct Film {
    char* judul;
    int harga;
    int kursi;
    Film* kiri;
    Film* kanan;
};

// === Stack untuk Undo ===
struct Undo {
    string aksi;
    char* judul;
    int harga;
    int kursi;
    Undo* next;
};

// === Struct Antrian (Queue) ===
struct Antrian {
    string nama;
    Antrian* next;
};

// === Global Pointer ===
Film* root = nullptr;
Undo* topUndo = nullptr;
Antrian* front = nullptr;
Antrian* rear = nullptr;

// === Fungsi Tambah Film ke BST ===
Film* buatFilm(const char* j, int harga, int kursi) {
    Film* baru = new Film;
    baru->judul = new char[strlen(j) + 1];
    strcpy(baru->judul, j);
    baru->harga = harga;
    baru->kursi = kursi;
    baru->kiri = baru->kanan = nullptr;
    return baru;
}

Film* tambahFilm(Film* akar, const char* j, int harga, int kursi) {
    if (!akar) {
        Undo* undo = new Undo{"TAMBAH", strdup(j), harga, kursi, topUndo};
        topUndo = undo;
        cout << "Film berhasil ditambahkan dan disimpan untuk undo.\n";
        return buatFilm(j, harga, kursi);
    }
    if (strcmp(j, akar->judul) < 0)
        akar->kiri = tambahFilm(akar->kiri, j, harga, kursi);
    else
        akar->kanan = tambahFilm(akar->kanan, j, harga, kursi);
    return akar;
}

// === In-order Tampilkan Film ===
void tampilkanFilm(Film* akar) {
    if (akar) {
        tampilkanFilm(akar->kiri);
        cout << akar->judul << " | Harga: " << akar->harga << " | Kursi: " << akar->kursi << endl;
        tampilkanFilm(akar->kanan);
    }
}

// === Cari terkecil untuk hapus ===
Film* cariTerkecil(Film* akar) {
    while (akar && akar->kiri) akar = akar->kiri;
    return akar;
}

// === Hapus Film ===
Film* hapusFilm(Film* akar, const char* j, Film*& filmTerhapus) {
    if (!akar) return nullptr;

    if (strcmp(j, akar->judul) < 0)
        akar->kiri = hapusFilm(akar->kiri, j, filmTerhapus);
    else if (strcmp(j, akar->judul) > 0)
        akar->kanan = hapusFilm(akar->kanan, j, filmTerhapus);
    else {
        filmTerhapus = buatFilm(akar->judul, akar->harga, akar->kursi);
        if (!akar->kiri) {
            Film* temp = akar->kanan;
            delete akar;
            return temp;
        }
        else if (!akar->kanan) {
            Film* temp = akar->kiri;
            delete akar;
            return temp;
        }
        Film* pengganti = cariTerkecil(akar->kanan);
        akar->judul = pengganti->judul;
        akar->harga = pengganti->harga;
        akar->kursi = pengganti->kursi;
        Film* dummy = nullptr;
        akar->kanan = hapusFilm(akar->kanan, pengganti->judul, dummy);
        delete dummy;
    }
    return akar;
}

void undo() {
    if (!topUndo) {
        cout << "Tidak ada aksi untuk di-undo.\n";
        return;
    }
    Undo* aksi = topUndo;
    topUndo = topUndo->next;

    if (aksi->aksi == "TAMBAH") {
        Film* dummy = nullptr;
        root = hapusFilm(root, aksi->judul, dummy);
        if (dummy) delete dummy;
        cout << "Undo: Film " << aksi->judul << " dihapus.\n";
    } else if (aksi->aksi == "HAPUS") {
        root = tambahFilm(root, aksi->judul, aksi->harga, aksi->kursi);
        cout << "Undo: Film " << aksi->judul << " dikembalikan.\n";
    }

    delete aksi;
}

// === Antrian ===
void enqueue(string nama) {
    Antrian* baru = new Antrian{nama, nullptr};
    if (rear == nullptr) front = rear = baru;
    else {
        rear->next = baru;
        rear = baru;
    }
    cout << nama << " telah masuk antrean.\n";
}

void dequeue() {
    if (front == nullptr) {
        cout << "Antrean kosong.\n";
        return;
    }
    Antrian* hapus = front;
    cout << hapus->nama << " dilayani.\n";
    front = front->next;
    if (front == nullptr) rear = nullptr;
    delete hapus;
}

void tampilkanAntrian() {
    Antrian* temp = front;
    int i = 1;
    while (temp != nullptr) {
        cout << i++ << ". " << temp->nama << endl;
        temp = temp->next;
    }
}

// === Menu ===
void menu() {
    int pilih;
    do {
        cout << "\n==== MENU FILM BIOSKOP ====" << endl;
        cout << "1. Tampilkan Film\n2. Tambah Film\n3. Hapus Film\n4. Undo\n5. Tambah Antrian\n6. Layani Antrian\n7. Tampilkan Antrian\n8. Keluar\nPilih: ";
        cin >> pilih;
        cin.ignore();

        if (pilih == 1) tampilkanFilm(root);
        else if (pilih == 2) {
            char judul[50]; int harga, kursi;
            cout << "Judul film: "; cin.getline(judul, 50);
            cout << "Harga tiket: "; cin >> harga;
            cout << "Jumlah kursi: "; cin >> kursi;
            cin.ignore();
            root = tambahFilm(root, judul, harga, kursi);
        }
        else if (pilih == 3) {
            char judul[50];
            cout << "Judul film yang akan dihapus: "; cin.getline(judul, 50);
            Film* terhapus = nullptr;
            root = hapusFilm(root, judul, terhapus);
            if (!terhapus) cout << "Film tidak ditemukan.\n";
            else {
                Undo* undo = new Undo{"HAPUS", strdup(judul), terhapus->harga, terhapus->kursi, topUndo};
                topUndo = undo;
                delete terhapus;
                cout << "Film berhasil dihapus dan disimpan untuk undo.\n";
            }
        }
        else if (pilih == 4) undo();
        else if (pilih == 5) {
            string nama;
            cout << "Nama: "; getline(cin, nama);
            enqueue(nama);
        }
        else if (pilih == 6) dequeue();
        else if (pilih == 7) tampilkanAntrian();
        else if (pilih == 8) cout << "Keluar...\n";
        else cout << "Pilihan tidak valid.\n";
    } while (pilih != 8);
}

int main() {
    menu();
    return 0;
}
