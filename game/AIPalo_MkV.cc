#include "Player.hh"


/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME Palo_MkV


struct PLAYER_NAME : public Player {

    /**
    * Factory: returns a new instance of this class.
    * Do not modify this function.
    */
    static Player* factory () {
        return new PLAYER_NAME;
    }

    /**
    * Types and attributes for your player can be defined here.
    */
    const int infinite = 60;
    typedef vector<vector<int> > Graph;
    typedef vector<vector<vector<int> > > Complex_Graph;
    Graph water;
    Graph station;
    Complex_Graph city;
    vector<Pos> city_pos;
    set<Pos> next_pos;
    Graph enemy;
    

    void water_distance (int i, int j) {
        water[i][j] = 0;
        vector<vector<bool> > visit(rows(), vector<bool>(cols(), false));
        queue <Pos> q;
        visit[i][j] = true;
        q.push(Pos(i,j));
        while (not q.empty()) {
            Pos p = q.front();
            q.pop();
            for (int k = 0; k < 8; ++k) {
                Pos r = p + Dir(k);
                if (pos_ok (r) and not visit[r.i][r.j]) {
                    visit[r.i][r.j] = true;
                    if (cell(r).type == Road or cell(r).type == Desert or cell(r).type == City) {
                        if (water[r.i][r.j] > water[p.i][p.j] + 1) water[r.i][r.j] = water[p.i][p.j] + 1;
                        q.push(r);
                    }
                }
            }
        }
    }
    
    void city_distance (int i, int j, int ncity) {
        city[i][j][ncity] = 0;
        vector<vector<bool> > visit(rows(), vector<bool>(cols(), false));
        queue <Pos> q;
        visit[i][j] = true;
        q.push(Pos(i,j));
        while (not q.empty()) {
            Pos p = q.front();
            q.pop();
            for (int k = 0; k < 8; ++k) {
                Pos r = p + Dir(k);
                if (pos_ok (r) and not visit[r.i][r.j]) {
                    visit[r.i][r.j] = true;
                    if (cell(r).type == Road or cell(r).type == Desert or cell(r).type == City) {
                        if (city[r.i][r.j][ncity] > city[p.i][p.j][ncity] + 1) city[r.i][r.j][ncity] = city[p.i][p.j][ncity] + 1;
                        q.push(r);
                    }
                }
            }           
        }
    }
    
    void visit_city(vector<vector<bool> >& v, int i, int j, int ncity) {
        v[i][j] = true;
        city_distance (i, j, ncity);
        Pos p(i,j);
        for (int k = 0; k < 8; ++k) {
            Pos r = p + Dir(k);
            if (pos_ok (r) and cell(r).type == City and not v[r.i][r.j]) {
                visit_city(v, r.i, r.j, ncity);
            }
        }
    }
    
    void station_distance (int i, int j) {
        station[i][j] = 0;
        vector<vector<bool> > visit(rows(), vector<bool>(cols(), false));
        queue <Pos> q;
        visit[i][j] = true;
        q.push(Pos(i,j));
        while (not q.empty()) {
            Pos p = q.front();
            q.pop();
            for (int k = 0; k < 8; ++k) {
                Pos r = p + Dir(k);
                if (pos_ok (r) and not visit[r.i][r.j]) {
                    visit[r.i][r.j] = true;
                    if (cell(r).type == Road) {
                        if (station[r.i][r.j] > station[p.i][p.j] + 1) station[r.i][r.j] = station[p.i][p.j] + 1;
                        q.push(r);
                    }
                }
            }
        }
    }
    
    
    int walk(int i, int j) {
        for (int k = 0; k < 8; ++k) {
            Pos r = Pos(i,j) + Dir(k);
            if (pos_ok (r) and cell(r).type == City and cell(r).id == -1 and next_pos.find(r) == next_pos.end()) return k;
        }
        return None;
    }
    
    Pos warrior_to_enemy(int i, int j) {
        vector<vector<bool> > visit(rows(), vector<bool>(cols(), false));
        queue <Pos> q;
        visit[i][j] = true;
        q.push(Pos(i,j));
        while (not q.empty()) {
            Pos p = q.front();
            q.pop();
            for (int k = 0; k < 8; ++k) {
                Pos r = p + Dir(k);
                if (pos_ok (r) and not visit[r.i][r.j]) {
                    visit[r.i][r.j] = true;
                    if (cell(r).type == City) {
                        //Acaba si troba un warrior d'un altre jugador
                        if (cell(r).id != -1 and unit(cell(r).id).player != me() and unit(cell(r).id).type != Car) return r;
                        q.push(r);
                    }
                }
            }
        }
        return Pos(i,j);
    }
    
    void warrior_enemy_distance (int i, int j) {
        enemy[i][j] = 0;
        vector<vector<bool> > visit(rows(), vector<bool>(cols(), false));
        queue <Pos> q;
        visit[i][j] = true;
        q.push(Pos(i,j));
        while (not q.empty()) {
            Pos p = q.front();
            q.pop();
            for (int k = 0; k < 8; ++k) {
                Pos r = p + Dir(k);
                if (pos_ok (r) and not visit[r.i][r.j]) {
                    visit[r.i][r.j] = true;
                    if (cell(r).type == City) {
                        if (enemy[r.i][r.j] > enemy[p.i][p.j] + 1) enemy[r.i][r.j] = enemy[p.i][p.j] + 1;
                        q.push(r);
                    }
                }
            }
        }
    }
    
    Pos car_to_enemy(int i, int j) {
        vector<vector<bool> > visit(rows(), vector<bool>(cols(), false));
        queue <Pos> q;
        visit[i][j] = true;
        q.push(Pos(i,j));
        while (not q.empty()) {
            Pos p = q.front();
            q.pop();
            for (int k = 0; k < 8; ++k) {
                Pos r = p + Dir(k);
                if (pos_ok (r) and not visit[r.i][r.j]) {
                    visit[r.i][r.j] = true;
                    if (cell(r).type == Road or next_to_road (r)) {
                        //Acaba si troba un warrior d'un altre jugador
                        if (cell(r).id != -1 and unit(cell(r).id).player != me() and unit(cell(r).id).type != Car) return r;
                        q.push(r);
                    }
                }
            }
        }
        return Pos(i,j);
    }
    
    bool next_to_road (Pos p) {
        for (int k = 0; k < 8; ++k) {
            Pos r = p + Dir(k);
            if (pos_ok (r) and cell(r).type == Road) return true;
        }
        return false;
    }
    
    void car_enemy_distance (int i, int j) {
        enemy[i][j] = 0;
        vector<vector<bool> > visit(rows(), vector<bool>(cols(), false));
        queue <Pos> q;
        visit[i][j] = true;
        q.push(Pos(i,j));
        while (not q.empty()) {
            Pos p = q.front();
            q.pop();
            for (int k = 0; k < 8; ++k) {
                Pos r = p + Dir(k);
                if (pos_ok (r) and not visit[r.i][r.j]) {
                    visit[r.i][r.j] = true;
                    if (cell(r).type == Road) {
                        if (enemy[r.i][r.j] > enemy[p.i][p.j] + 1) enemy[r.i][r.j] = enemy[p.i][p.j] + 1;
                        q.push(r);
                    }
                }
            }
        }
    }
    
    
    void init_game() {
        water = Graph(rows(), vector<int> (cols(), infinite));
        station = Graph(rows(), vector<int> (cols(), infinite));
        city = Complex_Graph(rows(), vector<vector<int> >(cols(), vector<int>(nb_cities(), infinite)));
        city_pos = vector<Pos>(nb_cities());
        vector<vector<bool> > visit(rows(), vector<bool>(cols(), false));
        int ncity = 0;
        for (int i = 0; i < rows(); ++i) {
            for (int j = 0; j < cols(); ++j) {
                if (cell(i, j).type == Water) water_distance (i, j);
                else if (cell(i, j).type == City) {
                    if (not visit[i][j]) {
                        visit_city(visit, i, j, ncity);
                        city_pos[ncity] = {i,j};
                        ++ncity;
                    }
                }
                else if (cell(i, j).type == Station) station_distance (i, j);
            }
        }
    }
    
    int count_defenders (int i, int j) {
        int defenders = 0;
        vector<vector<bool> > visit(rows(), vector<bool>(cols(), false));
        queue <Pos> q;
        visit[i][j] = true;
        q.push(Pos(i,j));
        while (not q.empty()) {
            Pos p = q.front();
            if (cell(p).id != -1 and unit(cell(p).id).player == me()) ++defenders;
            q.pop();
            for (int k = 0; k < 8; ++k) {
                Pos r = p + Dir(k);
                if (pos_ok (r) and not visit[r.i][r.j]) {
                    visit[r.i][r.j] = true;
                    if (cell(r).type == City) {
                        q.push(r);
                    }
                }
            }
        }
        return defenders;
    }
            
    int my_city(int i, int j) {
        vector<vector<bool> > visit(rows(), vector<bool>(cols(), false));
        queue <Pos> q;
        visit[i][j] = true;
        q.push(Pos(i,j));
        while (not q.empty()) {
            Pos p = q.front();
            for (int k = 0; k < nb_cities(); ++k) {
                if (p == city_pos[k]) return k;
            }
            q.pop();
            for (int k = 0; k < 8; ++k) {
                Pos r = p + Dir(k);
                if (pos_ok (r) and not visit[r.i][r.j]) {
                    visit[r.i][r.j] = true;
                    if (cell(r).type == City) {
                        q.push(r);
                    }
                }
            }
        }
        return -1;
    }
    
    bool car_here (Pos p) {
        for (int k = 0; k < 8; ++k) {
            Pos r = p + Dir(k);
            if (pos_ok (r) and cell(r).id != -1 and unit(cell(r).id).player != me() and unit(cell(r).id).type == Car) return true;
        }
        return false;
    }
    
    bool hard_pre (Pos p) {
        return (pos_ok (p) and cell(p).type == City and (cell(p).id == -1 or unit(cell(p).id).player != me()) and next_pos.find(p) == next_pos.end());
    }
    
    int enemies_away (Pos p) {
        for (int k = 0; k < 8; ++k) {
            Pos r = p + Dir(k);
            if (pos_ok (r) and cell(r).id != -1 and unit(cell(r).id).player != me() and unit(cell(r).id).type == Warrior) {
                if (k < 4) {
                    if (hard_pre(p+Dir(k + 4))) return (k + 4);
                    if (hard_pre(p+Dir(k + 3))) return (k + 3);
                    if (hard_pre(p+Dir((k + 5)%8))) return (k + 5)%8;
                } if (k >= 4) {
                    if (hard_pre(p+Dir(k - 4))) return (k - 4);
                    if (hard_pre(p+Dir(k - 3))) return (k - 3);
                    if (k-5 == -1 and hard_pre(p+Dir(7))) return 7; 
                    if (hard_pre(p+Dir(k - 5))) return (k - 5);
                }
            }
        }
        return None;
    }
    
    
    void move_warriors() {
        if (round()%4 != me()) return;
        vector<int> w = warriors(me());
        vector<int> defenders(nb_cities());
        for (int i = 0; i < nb_cities(); ++i) {
            if (cell(city_pos[i]).owner == me()) defenders[i] = count_defenders (city_pos[i].i, city_pos[i].j);
            else defenders[i] = 0;
        }
        for (int id : w) {
            //Si estem en una ciutat de la nostres posesio, quina es?
            int city_my = -1;
            if (cell(unit(id).pos).type == City and cell(unit(id).pos).owner == me()) city_my = my_city(unit(id).pos.i, unit(id).pos.j);
            //Si nececita aigua, ves a reomplir
            if (unit(id).water <= water[unit(id).pos.i][unit(id).pos.j] + 1) {
                int min_dist = infinite;
                int go = 8;
                for (int k = 0; k < 8; ++k) {
                    Pos p = unit(id).pos + Dir(k);
                    if (pos_ok(p) and min_dist > water[p.i][p.j] and (cell(p).id == -1 or unit(cell(p).id).player != me()) and next_pos.find(p) == next_pos.end()) {
                        go = k;
                        min_dist = water[p.i][p.j];
                    }
                }
                //Si estic a la ciutat i vui sortir pero hi ha un cotxe esperant -> no sortir
                if (cell(unit(id).pos).type == City and (cell(unit(id).pos + Dir(go)).type == City or cell(unit(id).pos + Dir(go)).type == Road) and car_here(unit(id).pos)) {
                    command(id, Dir(None));
                    next_pos.insert(unit(id).pos + Dir(None));
                } else {
                    command(id, Dir(go));
                    next_pos.insert(unit(id).pos + Dir(go));
                }
            }
            //Defensar les ciutats <- 2 warriors es queden defensant la ciutat
            //Mirem quants warriors tenim per ciutat i si son 4 o menys, defensem
            else if (cell(unit(id).pos).type == City and cell(unit(id).pos).owner == me() and defenders[city_my] <= 4 and defenders[city_my] >= 0) {
                //Defensar la ciutat: escapar-se dels enemics per a mantenir la posicio
                --defenders[city_my];
                int go = enemies_away(unit(id).pos);
                command(id, Dir(go));
                next_pos.insert(unit(id).pos + Dir(go));
            }
            //Si estem a una ciutat que no es nostre, derrotem als enemics per a conquerirla
            else if (cell(unit(id).pos).type == City and cell(unit(id).pos).owner != me()) {
                enemy = Graph(rows(), vector<int> (cols(), infinite));
                Pos e = warrior_to_enemy(unit(id).pos.i, unit(id).pos.j);
                int go = 8;
                if (e != unit(id).pos) {
                    warrior_enemy_distance (e.i, e.j);
                    int min_dist = infinite;
                    for (int k = 0; k < 8; ++k) {
                        Pos p = unit(id).pos + Dir(k);
                        if (pos_ok(p) and min_dist > enemy[p.i][p.j] and (cell(p).id == -1 or unit(cell(p).id).player != me()) and next_pos.find(p) == next_pos.end()) {
                            go = k;
                            min_dist = enemy[p.i][p.j];
                        }
                    }
                    
                }
                command(id, Dir(go));
                next_pos.insert(unit(id).pos + Dir(go));

            }
            //Conquerir noves ciutats
            else {
                int min_dist = infinite;
                int go = 8;
                for (int ncity = 0; ncity < nb_cities(); ++ncity) {
                    if (cell(city_pos[ncity]).owner != me()) {
                        for (int k = 0; k < 8; ++k) {
                            Pos p = unit(id).pos + Dir(k);
                            if (pos_ok(p) and min_dist > city[p.i][p.j][ncity] and (cell(p).id == -1 or (unit(cell(p).id).player != me() and unit(cell(p).id).type != Car)) and next_pos.find(p) == next_pos.end()) {
                                go = k;
                                min_dist = city[p.i][p.j][ncity];
                            }
                        }
                    }
                }
                command(id, Dir(go));
                next_pos.insert(unit(id).pos + Dir(go));
            }
        }
    }
    
    void move_cars() {
        vector<int> c = cars(me());
        for (int id : c) {
            if (can_move(id)) {
                //Si nececita gasoil, ves a reomplir
                if (unit(id).food <= station[unit(id).pos.i][unit(id).pos.j] + 5) {
                    int min_dist = infinite;
                    int go = 8;
                    for (int k = 0; k < 8; ++k) {
                        Pos p = unit(id).pos + Dir(k);
                        if (pos_ok(p) and min_dist > station[p.i][p.j] and (cell(p).id == -1 or (unit(cell(p).id).player != me() and unit(cell(p).id).type != Car)) and next_pos.find(p) == next_pos.end()) {
                            go = k;
                            min_dist = station[p.i][p.j];
                        }
                    }
                    command(id, Dir(go));
                    next_pos.insert(unit(id).pos + Dir(go));
                }
                //Busca enemic mes proper i ataca'l
                else {
                    enemy = Graph(rows(), vector<int> (cols(), infinite));
                    Pos e = car_to_enemy(unit(id).pos.i, unit(id).pos.j);
                    car_enemy_distance (e.i, e.j);
                    int min_dist = infinite;
                    int go = 8;
                    for (int k = 0; k < 8; ++k) {
                        Pos p = unit(id).pos + Dir(k);
                        if (pos_ok(p) and min_dist > enemy[p.i][p.j] and (cell(p).id == -1 or (unit(cell(p).id).player != me() and unit(cell(p).id).type != Car)) and next_pos.find(p) == next_pos.end()) {
                            go = k;
                            min_dist = enemy[p.i][p.j];
                        }
                    }
                    command(id, Dir(go));
                    next_pos.insert(unit(id).pos + Dir(go));
                }
            }
        }
    }

    /**
    * Play method, invoked once per each round.
    */
    virtual void play () {
        if (round() == 0) init_game();
        //water[-1][0] = 0;
        move_warriors();
        move_cars();
        next_pos.clear();
    }

};


/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);
