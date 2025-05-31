#include <iostream>
#include <cstring>
using namespace std;

// Queue node
struct QueueNode {
    char nama[50];
    QueueNode* next;
};

struct Queue {
    QueueNode* front;
    QueueNode* rear;

    Queue() {
        front = rear = NULL;
    }

    void enqueue(const char nama[]) {
        QueueNode* temp = new QueueNode;
        strcpy(temp->nama, nama);
        temp->next = NULL;
        if (rear == NULL) {
            front = rear = temp;
        } else {
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
        if (front != NULL) return front->nama;
        return NULL;
    }

    bool isEmpty() {
        return front == NULL;
    }
};

// Stack node
struct StackNode {
    char aksi[100];
    StackNode* next;
};

struct Stack {
    StackNode* top;

    Stack() {
        top = NULL;
    }

    void push(const char aksi[]) {
        StackNode* temp = new StackNode;
        strcpy(temp->aksi, aksi);
        temp->next = top;
        top = temp;
    }

    void pop() {
        if (top == NULL) return;
        StackNode* temp = top;
        top = top->next;
        delete temp;
    }

    const char* peek() {
        if (top != NULL) return top->aksi;
        return NULL;
    }

    bool isEmpty() {
        return top == NULL;
    }
};

// Pohon Telusur Biner berdasarkan Judul
struct Buku {
    char judul[50];
    bool tersedia;
    Queue antrian;
    Buku* kiri;
    Buku* kanan;
};

Buku* buatNode(const char judul[]) {
    Buku* baru = new Buku;
    strcpy(baru->judul, judul);
    baru->tersedia = true;
    baru->kiri = baru->kanan = NULL;
    return baru;
}

Buku* tambahBuku(Buku* root, const char judul[]) {
    if (root == NULL) return buatNode(judul);
    if (strcmp(judul, root->judul) < 0)
        root->kiri = tambahBuku(root->kiri, judul);
    else if (strcmp(judul, root->judul) > 0)
        root->kanan = tambahBuku(root->kanan, judul);
    else
        cout << "Buku sudah ada.\n";
    return root;
}

void inOrder(Buku* root) {
    if (root != NULL) {
        inOrder(root->kiri);
        cout << "Judul: " << root->judul << ", Status: " 
             << (root->tersedia ? "Tersedia" : "Dipinjam");
        if (!root->antrian.isEmpty()) {
            cout << " | Antrian: " << root->antrian.peek();
        }
        cout << endl;
        inOrder(root->kanan);
    }
}

Buku* cariBuku(Buku* root, const char judul[]) {
    if (root == NULL || strcmp(judul, root->judul) == 0)
        return root;
    if (strcmp(judul, root->judul) < 0)
        return cariBuku(root->kiri, judul);
    return cariBuku(root->kanan, judul);
}

void pinjamBuku(Buku* root, const char judul[], const char nama[], Stack& riwayat) {
    Buku* buku = cariBuku(root, judul);
    if (buku == NULL) {
        cout << "Buku tidak ditemukan.\n";
        return;
    }
    if (buku->tersedia) {
        buku->tersedia = false;
        cout << nama << " berhasil meminjam buku " << judul << endl;
        char aksi[100];
        sprintf(aksi, "Pinjam: %s oleh %s", judul, nama);
        riwayat.push(aksi);
    } else {
        cout << "Buku sedang dipinjam. " << nama << " masuk antrian.\n";
        buku->antrian.enqueue(nama);
    }
}

void kembalikanBuku(Buku* root, const char judul[], Stack& riwayat) {
    Buku* buku = cariBuku(root, judul);
    if (buku == NULL) {
        cout << "Buku tidak ditemukan.\n";
        return;
    }
    if (!buku->tersedia) {
        if (!buku->antrian.isEmpty()) {
            const char* next = buku->antrian.peek();
            cout << "Buku dikembalikan. Sekarang dipinjam oleh " << next << endl;
            buku->antrian.dequeue();
            // Tetap tidak tersedia karena langsung dipinjam lagi
        } else {
            buku->tersedia = true;
            cout << "Buku berhasil dikembalikan dan sekarang tersedia.\n";
        }
        char aksi[100];
        sprintf(aksi, "Kembali: %s", judul);
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

int main() {
    Buku* root = NULL;
    Stack riwayat;

    // Tambah buku
    root = tambahBuku(root, "Struktur Data");
    root = tambahBuku(root, "Basis Data");
    root = tambahBuku(root, "Algoritma");

    // Tampilkan semua buku
    cout << "\nDaftar Buku:\n";
    inOrder(root);

    // Peminjaman
    cout << "\nProses Peminjaman:\n";
    pinjamBuku(root, "Struktur Data", "Andi", riwayat);
    pinjamBuku(root, "Struktur Data", "Budi", riwayat);

    // Pengembalian
    cout << "\nProses Pengembalian:\n";
    kembalikanBuku(root, "Struktur Data", riwayat);

    // Undo aksi terakhir
    cout << "\nUndo:\n";
    undo(riwayat);

    // Tampilkan akhir
    cout << "\nDaftar Buku Terakhir:\n";
    inOrder(root);

    return 0;
}
