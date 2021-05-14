
void deg0() { // Shape only
    G3Xpoint I;
    G3Xvector N;
    Object *obj = objects;
    do {
        RayIntersection(R, obj, &I, &N, false);
    } while ((obj = obj->next) != objects);

    if (R->object != NULL) R->color = R->object->color;
}
void deg1() { // Interaction with shape
    G3Xpoint I;
    G3Xvector N;
    Object *obj = objects;
    do {
        RayIntersection(R, obj, &I, &N, false);
    } while ((obj = obj->next) != objects);

    if (R->object == NULL) return;

    G3Xvector w = g3x_SetVect(I, L);
    g3x_Normalize(&w);
    R->color = multColor(R->object->color, CLIP(0, g3x_ProdScal(N, w), 1));
}
void deg2() { // Ambient & Diffuse light
    G3Xpoint I;
    G3Xvector N;
    Object *obj = objects;
    do {
        RayIntersection(R, obj, &I, &N, false);
    } while ((obj = obj->next) != objects);
    if (R->object == NULL) return;

    G3Xvector w = g3x_SetVect(I, L);
    g3x_Normalize(&w);

    R->color = multColor(R->object->color, R->object->mat.ambi);
    double ps = g3x_ProdScal(N, w);
    if (ps <= 0) return;
    R->color = sumColor(multColor(R->object->color, R->object->mat.diff * ps), R->color);
}
void deg3() { // Reflexion
    if (rec <= 0) return;
    rec -= 1;

    G3Xpoint I;
    G3Xvector N;
    Object *obj = objects;
    do {
        RayIntersection(R, obj, &I, &N, false);
    } while ((obj = obj->next) != objects);
    if (R->object == NULL) return;

    G3Xvector w = g3x_SetVect(I, L);
    g3x_Normalize(&w);

    double ps = g3x_ProdScal(N, w);
    if (ps <= 0) {
        R->color = compute_final_color(R->object->mat, R->object->color, G3Xw, G3Xk, G3Xk, ps, 0);
        return;
    }

    Rayon refl = Cree_Rayon(I, g3x_SubVect(R->direction, g3x_mapscal3(N, 2 * g3x_ProdScal(N, R->direction))));
    RayTracer(&refl, objects, L, rec);

    double ps_uWr = -g3x_ProdScal(R->direction, reflectVector(w, N));
    R->color = compute_final_color(R->object->mat, R->object->color, G3Xw, refl.color, G3Xk, ps, ps_uWr);
}
void deg4() { // Refraction ON GOING
    if (rec <= 0) return;
    rec -= 1;

    G3Xpoint I;
    G3Xvector N;
    Object *obj = objects;
    do {
        RayIntersection(R, obj, &I, &N, false);
    } while ((obj = obj->next) != objects);
    if (R->object == NULL) return;

    G3Xvector w = g3x_SetVect(I, L);
    g3x_Normalize(&w);
    double ps = g3x_ProdScal(N, w);
    Rayon refl;
    double ps_uWr;
    if (ps <= 0) {
        R->color = compute_final_color(R->object->mat, R->object->color, G3Xw, G3Xk, G3Xk, ps, 0);
    } else {
        refl = Cree_Rayon(I, g3x_SubVect(R->direction, g3x_mapscal3(N, 2 * g3x_ProdScal(N, R->direction))));
        RayTracer(&refl, objects, L, rec);
        ps_uWr = -g3x_ProdScal(R->direction, reflectVector(w, N));

        R->color = compute_final_color(R->object->mat, R->object->color, G3Xw, refl.color, G3Xk, ps, ps_uWr);
    }
    G3Xvector ut = refractVector(R->direction, N, 1 / R->object->mat.n);
    Rayon T = Cree_Rayon(I, ut);
    T.color = (G3Xcolor) {0, 1, 0, 0};

    G3Xpoint J;
    G3Xvector M;
    RayIntersection(&T, objects, &J, &M, true);

    M = (G3Xvector) {-M.x, -M.y, -M.z};
    G3Xvector vt = refractVector(T.direction, M, R->object->mat.n / 1);
    Rayon S = Cree_Rayon(J, vt);
    S.color = (G3Xcolor) {0, 0, 1, 0};
    T.color = (G3Xcolor) {T.object == NULL ? 0 : 1, 1, 0, 0};
    Draw_Rayon(&T);
    Draw_Rayon(&S);
    Draw_Rayon(R);

    R->color = compute_final_color(R->object->mat, R->object->color, G3Xw, refl.color, T.object->color, ps, ps_uWr);
}
void all_deg() {

}


void RayTracer(Rayon *R, Object *objects, G3Xpoint L, int rec) {
    if (rec <= 0) return;
    rec -= 1;

    G3Xpoint I;
    G3Xvector N;
    Object *obj = objects;
    do {
        RayIntersection(R, obj, &I, &N, false);
    } while ((obj = obj->next) != objects);

    if (R->object == NULL) return;

    R->color = R->object->color;

    if (RAYTRACER_DEG < 1) return;
    G3Xvector w = g3x_SetVect(I, L);
    g3x_Normalize(&w);

    double prod_scal = g3x_ProdScal(N, w);

    if (prod_scal <= 0) {
        if (RAYTRACER_DEG <= 1) {
            R->color = G3Xk;
        } else {
            R->color = shadeColor(R->object->color, R->object->mat.ambi);
        }
    } else {
        if (RAYTRACER_DEG <= 1) {
            R->color = shadeColor(R->object->color, prod_scal);
        } else {
            R->color = colorSum(shadeColor(R->object->color, R->object->mat.diff * prod_scal),
                                shadeColor(R->object->color, R->object->mat.ambi));
        }
    }

    if (RAYTRACER_DEG < 3) return;

    G3Xvector sym_w = reflectRayon(w, N);

    G3Xcolor shine = shadeColor(G3Xw,
                                R->object->mat.shin * R->object->mat.spec *
                                MAX(0,
                                    pow(-g3x_ProdScal(R->direction, sym_w), (double) (1. / (1 - R->object->mat.shin))))
    );


    R->color = colorSum(R->color, shine);

    G3Xvector refl_dir = g3x_SubVect(R->direction, g3x_mapscal3(N, 2 * g3x_ProdScal(N, R->direction)));
    Rayon refl = Cree_Rayon(I, refl_dir);

    RayTracer(&refl, objects, L, rec);
    R->color = colorSum(R->color, shadeColor(refl.color, R->object->mat.shin));

    Draw_Rayon(R);
    if (RAYTRACER_DEG < 4) return;

    G3Xvector ut = VctTransmis(R->direction, N, 1 / R->object->mat.n);
    Rayon T = Cree_Rayon(I, ut);
    T.color = (G3Xcolor) {0, 1, 0, 0};
    Draw_Rayon(&T);

    G3Xpoint J;
    G3Xvector M;
    RayIntersection(&T, objects, &J, &M, true);

    M = (G3Xvector) {-M.x, -M.y, -M.z};
    G3Xvector vt = VctTransmis(T.direction, M, R->object->mat.n / 1);
    Rayon S = Cree_Rayon(J, vt);
    S.color = (G3Xcolor) {0, 0, 1, 0};
    Draw_Rayon(&S);

    if (T.object == NULL) return;

    T.color = T.object->color;
    R->color = T.color;
}
