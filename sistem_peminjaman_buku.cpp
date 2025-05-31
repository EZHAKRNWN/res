#include <iostream>
#include <cstring>
using namespace std;

// Struktur Queue
struct QueueNode {
    char nama[50];
    QueueNode* next;
};

struct Queue {
    QueueNode* front, *rear;
    Queue() { front = rear = NULL; }

    void enqueue(const char nama[]) {
        QueueNode* temp = new QueueNode;
        strcpy(temp->nama, nama);
        temp->next = NULL;
        if (rear == NULL)
            front = rear = temp;
        else {
            rear->next = temp;
            rear = temp;
        }
    }

    void dequeue() {
        if (front == NULL) return;
        QueueNode* temp = front;
        front = front->next;
        if (front == NULL) rear = NULL;
        delete temp;
    }

    const char* peek() {
        return front ? front->nama : NULL;
    }

    bool isEmpty() {
        return front == NULL;
    }
};

// Struktur Stack
struct StackNode {
    char aksi[100];
    StackNode* next;
};

struct Stack {
    StackNode* top;
    Stack() { top = NULL; }

    void push(const char aksi[]) {
        StackNode* temp = new StackNode;
        strcpy(temp->aksi, aksi);
        temp->next = top;
        top = temp;
    }

    void pop() {
        if (!top) return;
        StackNode* temp = top;
        top = top->next;
        delete temp;
    }

    const char* peek() {
        return top ? top->aksi : NULL;
    }

    bool isEmpty() {
        return top == NULL;
    }
};

// Struktur Buku (PTB)
struct Buku {
    char judul[50];
    bool tersedia;
    Queue antrian;
    Buku *kiri, *kanan;
};

Buku* buatNode(const char judul[]) {
    Buku* baru = new Buku;
    strcpy(baru->judul, judul);
    baru->tersedia = true;
    baru->kiri = baru->kanan = NULL;
    return baru;
}

Buku* tambahBuku(Buku* root, const char judul[]) {
    if (!root) return buatNode(judul);
    if (strcmp(judul, root->judul) < 0)
        root->kiri = tambahBuku(root->kiri, judul);
    else if (strcmp(judul, root->judul) > 0)
        root->kanan = tambahBuku(root->kanan, judul);
    else
        cout << "Buku sudah ada.\n";
    return root;
}

void inOrder(Buku* root) {
    if (root) {
        inOrder(root->kiri);
        cout << "Judul: " << root->judul
             << " | Status: " << (root->tersedia ? "Tersedia" : "Dipinjam");
        if (!root->antrian.isEmpty())
            cout << " | Antrian: " << root->antrian.peek();
        cout << endl;
        inOrder(root->kanan);
    }
}

Buku* cariBuku(Buku* root, const char judul[]) {
    if (!root || strcmp(judul, root->judul) == 0) return root;
    if (strcmp(judul, root->judul) < 0)
        return cariBuku(root->kiri, judul);
    return cariBuku(root->kanan, judul);
}

void pinjamBuku(Buku* root, const char judul[], const char nama[], Stack& riwayat) {
    Buku* buku = cariBuku(root, judul);
    if (!buku) {
        cout << "Buku tidak ditemukan.\n";
        return;
    }
    if (buku->tersedia) {
        buku->tersedia = false;
        cout << nama << " berhasil meminjam buku " << judul << endl;
        char aksi[100]; sprintf(aksi, "Pinjam: %s oleh %s", judul, nama);
        riwayat.push(aksi);
    } else {
        cout << "Buku sedang dipinjam. " << nama << " masuk antrian.\n";
        buku->antrian.enqueue(nama);
    }
}

void kembalikanBuku(Buku* root, const char judul[], Stack& riwayat) {
    Buku* buku = cariBuku(root, judul);
    if (!buku) {
        cout << "Buku tidak ditemukan.\n";
        return;
    }
    if (!buku->tersedia) {
        if (!buku->antrian.isEmpty()) {
            const char* next = buku->antrian.peek();
            cout << "Buku dikembalikan. Sekarang dipinjam oleh " << next << endl;
            buku->antrian.dequeue();
        } else {
            buku->tersedia = true;
            cout << "Buku berhasil dikembalikan dan tersedia.\n";
        }
        char aksi[100]; sprintf(aksi, "Kembali: %s", judul);
        riwayat.push(aksi);
    } else {
        cout << "Buku belum dipinjam.\n";
    }
}

void undo(Stack& riwayat) {
    if (!riwayat.isEmpty()) {
        cout << "Undo aksi: " << riwayat.peek() << endl;
        riwayat.pop();
    } else {
        cout << "Tidak ada aksi untuk di-undo.\n";
    }
}

Buku* cariMin(Buku* root) {
    while (root && root->kiri) root = root->kiri;
    return root;
}

Buku* hapusBuku(Buku* root, const char judul[]) {
    if (!root) return NULL;
    if (strcmp(judul, root->judul) < 0)
        root->kiri = hapusBuku(root->kiri, judul);
    else if (strcmp(judul, root->judul) > 0)
        root->kanan = hapusBuku(root->kanan, judul);
    else {
        if (!root->kiri) {
            Buku* temp = root->kanan;
            delete root;
            return temp;
        } else if (!root->kanan) {
            Buku* temp = root->kiri;
            delete root;
            return temp;
        }
        Buku* temp = cariMin(root->kanan);
        strcpy(root->judul, temp->judul);
        root->tersedia = temp->tersedia;
        root->kanan = hapusBuku(root->kanan, temp->judul);
    }
    return root;
}

int main() {
    Buku* root = NULL;
    Stack riwayat;
    int pilihan;
    char judul[50], nama[50];

    do {
        cout << "\n=== Menu Sistem Peminjaman Buku ===\n";
        cout << "1. Tambah Buku\n";
        cout << "2. Tampilkan Buku\n";
        cout << "3. Pinjam Buku\n";
        cout << "4. Kembalikan Buku\n";
        cout << "5. Undo Aksi\n";
        cout << "6. Hapus Buku\n";
        cout << "0. Keluar\n";
        cout << "Pilihan: ";
        cin >> pilihan; cin.ignore();

        switch (pilihan) {
            case 1:
                cout << "Judul buku: "; cin.getline(judul, 50);
                root = tambahBuku(root, judul);
                break;
            case 2:
                cout << "Daftar Buku:\n";
                inOrder(root);
                break;
            case 3:
                cout << "Judul buku: "; cin.getline(judul, 50);
                cout << "Nama peminjam: "; cin.getline(nama, 50);
                pinjamBuku(root, judul, nama, riwayat);
                break;
            case 4:
                cout << "Judul buku: "; cin.getline(judul, 50);
                kembalikanBuku(root, judul, riwayat);
                break;
            case 5:
                undo(riwayat);
                break;
            case 6:
                cout << "Judul buku yang akan dihapus: "; cin.getline(judul, 50);
                root = hapusBuku(root, judul);
                cout << "Buku dihapus.\n";
                break;
            case 0:
                cout << "Keluar dari program.\n"; break;
            default:
                cout << "Pilihan tidak valid!\n";
        }
    } while (pilihan != 0);

    return 0;
}
