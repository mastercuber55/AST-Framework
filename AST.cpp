#include "AST.h"
#include <thread>
#include <iostream>
#include <stdlib.h>
#include <math.h>

// ############### SKR_Physics.cpp Start ############### 
#ifdef AST_PHYSICS
struct SKR_StaticRect {
	SDL_FRect* position;
	SKR_StaticRect* sonraki;
};

struct SKR_KinematicRect {
	SDL_FRect* position;
	SDL_FPoint velocity;
	SKR_KinematicRect* sonraki;
	int boolean;
};

struct SKR_DynamicRect {
	SDL_FRect* position;
	float mass;
	SDL_FPoint velocity;
	float xk;
	float yk;
	SDL_FPoint force;
	SKR_DynamicRect* sonraki;
	int isonground;
	float friction;
	int alt, ust, sol, sag;
	float gravitymultiplier;
};

struct SKR_RectWorld {
	float gravity;
	float airfriction;
	SKR_GAMETYPE gametype;
	SKR_StaticRect* StaticRectList;
	SKR_KinematicRect* KinematicRectList;
	SKR_DynamicRect* DynamicRectList;
};

float* xmax = NULL, * xmin = NULL, * ymax = NULL, * ymin = NULL;

float Xmax, Ymax;

float xmax2, ymax2, xmin2;

int loop, i;

SDL_FPoint a, b, c, d;

SKR_DynamicRect* tmpD;
SKR_DynamicRect* tmpD2;
SKR_StaticRect* tmpS;
SKR_KinematicRect* tmpK;

int onSegment(SDL_FPoint p, SDL_FPoint q, SDL_FPoint r) {

	if (q.x <= max(p.x, r.x) && q.x >= min(p.x, r.x) && q.y <= max(p.y, r.y) && q.y >= min(p.y, r.y)) {
		return 1;
	}

	return 0;
}

int orientation(SDL_FPoint p, SDL_FPoint q, SDL_FPoint r) {

	if (((q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y)) == 0) {
		return 0;
	}

	return (((q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y)) > 0) ? 1 : 2;

}

int doIntersect(SDL_FPoint p1, SDL_FPoint q1, SDL_FPoint p2, SDL_FPoint q2) {

	if (orientation(p1, q1, p2) != orientation(p1, q1, q2) && orientation(p2, q2, p1) != orientation(p2, q2, q1)) { return 1; }

	if (orientation(p1, q1, p2) == 0 && onSegment(p1, p2, q1)) { return 1; }

	if (orientation(p1, q1, q2) == 0 && onSegment(p1, q2, q1)) { return 1; }

	if (orientation(p2, q2, p1) == 0 && onSegment(p2, p1, q2)) { return 1; }

	if (orientation(p2, q2, q1) == 0 && onSegment(p2, q1, q2)) { return 1; }

	return 0;
}

int SKR_IntersectRectLine(SDL_FRect* Rect, float* x1, float* y1, float* x2, float* y2) {
	a.x = *x1;
	a.y = *y1;
	b.x = *x2;
	b.y = *y2;
	c.x = Rect->x;
	c.y = Rect->y;
	d.x = Rect->x + Rect->w;
	d.y = Rect->y;
	if (doIntersect(a, b, c, d)) {
		return 1;
	}
	d.x = Rect->x;
	d.y = Rect->y + Rect->h;
	if (doIntersect(a, b, c, d)) {
		return 1;
	}
	c.x = Rect->x + Rect->w;
	c.y = Rect->y + Rect->h;
	if (doIntersect(a, b, c, d)) {
		return 1;
	}
	d.x = Rect->x + Rect->w;
	d.y = Rect->y;
	if (doIntersect(a, b, c, d)) {
		return 1;
	}
	if (Rect->x <= min(*x1, *x2) && (Rect->x + Rect->w) >= max(*x1, *x2) && Rect->y <= min(*y1, *y2) && (Rect->y + Rect->h) >= max(*y1, *y2)) {
		return 1;
	}
	return 0;
}

int SKR_IntersectRectRect(SDL_FRect* Rect1, SDL_FRect* Rect2) {
	return (Rect1->x <= (Rect2->x + Rect2->w) && Rect2->x <= (Rect1->x + Rect1->w) && Rect1->y <= (Rect2->y + Rect2->h) && Rect2->y <= (Rect1->y + Rect1->h));
}

void CollideDynamicStatic(SKR_DynamicRect* DynamicRect, SKR_StaticRect* StaticRect) {
	if (SKR_IntersectRectRect(DynamicRect->position, StaticRect->position)) {
		*xmin = StaticRect->position->x;
		*ymin = StaticRect->position->y;
		*xmax = StaticRect->position->x + StaticRect->position->w;
		*ymax = StaticRect->position->y + StaticRect->position->h;
		if (SKR_IntersectRectLine(DynamicRect->position, xmin, ymin, xmax, ymin) && !SKR_IntersectRectLine(DynamicRect->position, xmax, ymin, xmax, ymax) && !SKR_IntersectRectLine(DynamicRect->position, xmax, ymax, xmin, ymax) && !SKR_IntersectRectLine(DynamicRect->position, xmin, ymax, xmin, ymin)) {
			DynamicRect->position->y = StaticRect->position->y - DynamicRect->position->h - 0.01f;
			if (DynamicRect->velocity.y > 0) {
				DynamicRect->velocity.y = 0;
			}
			DynamicRect->isonground = 1;
			DynamicRect->alt = 1;
		}
		else if (!SKR_IntersectRectLine(DynamicRect->position, xmin, ymin, xmax, ymin) && SKR_IntersectRectLine(DynamicRect->position, xmax, ymin, xmax, ymax) && !SKR_IntersectRectLine(DynamicRect->position, xmax, ymax, xmin, ymax) && !SKR_IntersectRectLine(DynamicRect->position, xmin, ymax, xmin, ymin)) {
			DynamicRect->position->x = StaticRect->position->x + StaticRect->position->w + 0.01f;
			if (DynamicRect->velocity.x < 0) {
				DynamicRect->velocity.x = 0;
			}
			DynamicRect->sol = 1;
		}
		else if (!SKR_IntersectRectLine(DynamicRect->position, xmin, ymin, xmax, ymin) && !SKR_IntersectRectLine(DynamicRect->position, xmax, ymin, xmax, ymax) && SKR_IntersectRectLine(DynamicRect->position, xmax, ymax, xmin, ymax) && !SKR_IntersectRectLine(DynamicRect->position, xmin, ymax, xmin, ymin)) {
			DynamicRect->position->y = StaticRect->position->y + StaticRect->position->h + 0.01f;
			if (DynamicRect->velocity.y < 0) {
				DynamicRect->velocity.y = 0;
			}
			DynamicRect->ust = 1;
		}
		else if (!SKR_IntersectRectLine(DynamicRect->position, xmin, ymin, xmax, ymin) && !SKR_IntersectRectLine(DynamicRect->position, xmax, ymin, xmax, ymax) && !SKR_IntersectRectLine(DynamicRect->position, xmax, ymax, xmin, ymax) && SKR_IntersectRectLine(DynamicRect->position, xmin, ymax, xmin, ymin)) {
			DynamicRect->position->x = StaticRect->position->x - DynamicRect->position->w - 0.01f;
			if (DynamicRect->velocity.x > 0) {
				DynamicRect->velocity.x = 0;
			}
			DynamicRect->sag = 1;
		}
		else if (SKR_IntersectRectLine(DynamicRect->position, xmin, ymin, xmax, ymin) && SKR_IntersectRectLine(DynamicRect->position, xmax, ymin, xmax, ymax) && !SKR_IntersectRectLine(DynamicRect->position, xmax, ymax, xmin, ymax) && !SKR_IntersectRectLine(DynamicRect->position, xmin, ymax, xmin, ymin)) {
			if ((StaticRect->position->x + StaticRect->position->w - DynamicRect->position->x) >= ((DynamicRect->position->y + DynamicRect->position->h - StaticRect->position->y))) {
				DynamicRect->position->y = StaticRect->position->y - DynamicRect->position->h - 0.01f;
				if (DynamicRect->velocity.y > 0) {
					DynamicRect->velocity.y = 0;
				}
				DynamicRect->isonground = 1;
				DynamicRect->alt = 1;
			}
			else {
				DynamicRect->position->x = StaticRect->position->x + StaticRect->position->w + 0.01f;
				if (DynamicRect->velocity.x < 0) {
					DynamicRect->velocity.x = 0;
				}
				DynamicRect->sol = 1;
			}
		}
		else if (!SKR_IntersectRectLine(DynamicRect->position, xmin, ymin, xmax, ymin) && SKR_IntersectRectLine(DynamicRect->position, xmax, ymin, xmax, ymax) && SKR_IntersectRectLine(DynamicRect->position, xmax, ymax, xmin, ymax) && !SKR_IntersectRectLine(DynamicRect->position, xmin, ymax, xmin, ymin)) {
			if ((StaticRect->position->x + StaticRect->position->w - DynamicRect->position->x) >= ((StaticRect->position->y + StaticRect->position->h - DynamicRect->position->y) + DynamicRect->velocity.y)) {
				DynamicRect->position->y = StaticRect->position->y + StaticRect->position->h + 0.01f;
				if (DynamicRect->velocity.y < 0) {
					DynamicRect->velocity.y = 0;
				}
				DynamicRect->ust = 1;
			}
			else {
				DynamicRect->position->x = StaticRect->position->x + StaticRect->position->w + 0.01f;
				if (DynamicRect->velocity.x < 0) {
					DynamicRect->velocity.x = 0;
				}
				DynamicRect->sol = 1;
			}
		}
		else if (!SKR_IntersectRectLine(DynamicRect->position, xmin, ymin, xmax, ymin) && !SKR_IntersectRectLine(DynamicRect->position, xmax, ymin, xmax, ymax) && SKR_IntersectRectLine(DynamicRect->position, xmax, ymax, xmin, ymax) && SKR_IntersectRectLine(DynamicRect->position, xmin, ymax, xmin, ymin)) {
			if ((DynamicRect->position->x + DynamicRect->position->w - StaticRect->position->x) >= ((StaticRect->position->y + StaticRect->position->h - DynamicRect->position->y) + DynamicRect->velocity.y)) {
				DynamicRect->position->y = StaticRect->position->y + StaticRect->position->h + 0.01f;
				if (DynamicRect->velocity.y < 0) {
					DynamicRect->velocity.y = 0;
				}
				DynamicRect->ust = 1;
			}
			else {
				DynamicRect->position->x = StaticRect->position->x - DynamicRect->position->w - 0.01f;
				if (DynamicRect->velocity.x > 0) {
					DynamicRect->velocity.x = 0;
				}
				DynamicRect->sag = 1;
			}
		}
		else if (SKR_IntersectRectLine(DynamicRect->position, xmin, ymin, xmax, ymin) && !SKR_IntersectRectLine(DynamicRect->position, xmax, ymin, xmax, ymax) && !SKR_IntersectRectLine(DynamicRect->position, xmax, ymax, xmin, ymax) && SKR_IntersectRectLine(DynamicRect->position, xmin, ymax, xmin, ymin)) {
			if ((DynamicRect->position->x + DynamicRect->position->w - StaticRect->position->x) >= ((DynamicRect->position->y + DynamicRect->position->h - StaticRect->position->y))) {
				DynamicRect->position->y = StaticRect->position->y - DynamicRect->position->h - 0.01f;
				if (DynamicRect->velocity.y > 0) {
					DynamicRect->velocity.y = 0;
				}
				DynamicRect->isonground = 1;
				DynamicRect->alt = 1;
			}
			else {
				DynamicRect->position->x = StaticRect->position->x - DynamicRect->position->w - 0.01f;
				if (DynamicRect->velocity.x > 0) {
					DynamicRect->velocity.x = 0;
				}
				DynamicRect->sag = 1;
			}
		}
		else if (!SKR_IntersectRectLine(DynamicRect->position, xmin, ymin, xmax, ymin) && SKR_IntersectRectLine(DynamicRect->position, xmax, ymin, xmax, ymax) && SKR_IntersectRectLine(DynamicRect->position, xmax, ymax, xmin, ymax) && SKR_IntersectRectLine(DynamicRect->position, xmin, ymax, xmin, ymin)) {
			DynamicRect->position->y = StaticRect->position->y + StaticRect->position->h + 0.01f;
			if (DynamicRect->velocity.y < 0) {
				DynamicRect->velocity.y = 0;
			}
			DynamicRect->ust = 1;
		}
		else if (SKR_IntersectRectLine(DynamicRect->position, xmin, ymin, xmax, ymin) && !SKR_IntersectRectLine(DynamicRect->position, xmax, ymin, xmax, ymax) && SKR_IntersectRectLine(DynamicRect->position, xmax, ymax, xmin, ymax) && SKR_IntersectRectLine(DynamicRect->position, xmin, ymax, xmin, ymin)) {
			DynamicRect->position->x = StaticRect->position->x - DynamicRect->position->w - 0.01f;
			if (DynamicRect->velocity.x > 0) {
				DynamicRect->velocity.x = 0;
			}
			DynamicRect->sag = 1;
		}
		else if (SKR_IntersectRectLine(DynamicRect->position, xmin, ymin, xmax, ymin) && SKR_IntersectRectLine(DynamicRect->position, xmax, ymin, xmax, ymax) && !SKR_IntersectRectLine(DynamicRect->position, xmax, ymax, xmin, ymax) && SKR_IntersectRectLine(DynamicRect->position, xmin, ymax, xmin, ymin)) {
			DynamicRect->position->y = StaticRect->position->y - DynamicRect->position->h - 0.01f;
			if (DynamicRect->velocity.y > 0) {
				DynamicRect->velocity.y = 0;
			}
			DynamicRect->isonground = 1;
			DynamicRect->alt = 1;
		}
		else if (SKR_IntersectRectLine(DynamicRect->position, xmin, ymin, xmax, ymin) && SKR_IntersectRectLine(DynamicRect->position, xmax, ymin, xmax, ymax) && SKR_IntersectRectLine(DynamicRect->position, xmax, ymax, xmin, ymax) && !SKR_IntersectRectLine(DynamicRect->position, xmin, ymax, xmin, ymin)) {
			DynamicRect->position->x = StaticRect->position->x + StaticRect->position->w + 0.01f;
			if (DynamicRect->velocity.x < 0) {
				DynamicRect->velocity.x = 0;
			}
			DynamicRect->sol = 1;
		}
	}
	return;
}

void CollideDynamicKinematic(SKR_DynamicRect* DynamicRect, SKR_KinematicRect* KinematicRect, float gravity, SKR_GAMETYPE gametype) {
	if (SKR_IntersectRectRect(DynamicRect->position, KinematicRect->position)) {
		*xmin = KinematicRect->position->x;
		*ymin = KinematicRect->position->y;
		*xmax = KinematicRect->position->x + KinematicRect->position->w;
		*ymax = KinematicRect->position->y + KinematicRect->position->h;
		if (SKR_IntersectRectLine(DynamicRect->position, xmin, ymin, xmax, ymin) && !SKR_IntersectRectLine(DynamicRect->position, xmax, ymin, xmax, ymax) && !SKR_IntersectRectLine(DynamicRect->position, xmax, ymax, xmin, ymax) && !SKR_IntersectRectLine(DynamicRect->position, xmin, ymax, xmin, ymin)) {
			DynamicRect->position->y = KinematicRect->position->y - DynamicRect->position->h - 0.01f;
			if (DynamicRect->velocity.y > 0) {
				DynamicRect->velocity.y = 0;
			}
			DynamicRect->alt = 1;
			if (gametype == SKR_SIDESCROLLER) {
				DynamicRect->isonground = 1;
				DynamicRect->position->y += 0.02f;
				if (KinematicRect->velocity.y > 0 && KinematicRect->velocity.y <= gravity) {
					DynamicRect->yk = KinematicRect->velocity.y;
				}
				DynamicRect->xk = KinematicRect->velocity.x;
			}
		}
		else if (!SKR_IntersectRectLine(DynamicRect->position, xmin, ymin, xmax, ymin) && SKR_IntersectRectLine(DynamicRect->position, xmax, ymin, xmax, ymax) && !SKR_IntersectRectLine(DynamicRect->position, xmax, ymax, xmin, ymax) && !SKR_IntersectRectLine(DynamicRect->position, xmin, ymax, xmin, ymin)) {
			DynamicRect->position->x = KinematicRect->position->x + KinematicRect->position->w + 0.01f;
			if (DynamicRect->velocity.x < 0) {
				DynamicRect->velocity.x = 0;
			}
			DynamicRect->sol = 1;
		}
		else if (!SKR_IntersectRectLine(DynamicRect->position, xmin, ymin, xmax, ymin) && !SKR_IntersectRectLine(DynamicRect->position, xmax, ymin, xmax, ymax) && SKR_IntersectRectLine(DynamicRect->position, xmax, ymax, xmin, ymax) && !SKR_IntersectRectLine(DynamicRect->position, xmin, ymax, xmin, ymin)) {
			DynamicRect->position->y = KinematicRect->position->y + KinematicRect->position->h + 0.01f;
			if (DynamicRect->velocity.y < 0) {
				DynamicRect->velocity.y = 0;
			}
			DynamicRect->ust = 1;
		}
		else if (!SKR_IntersectRectLine(DynamicRect->position, xmin, ymin, xmax, ymin) && !SKR_IntersectRectLine(DynamicRect->position, xmax, ymin, xmax, ymax) && !SKR_IntersectRectLine(DynamicRect->position, xmax, ymax, xmin, ymax) && SKR_IntersectRectLine(DynamicRect->position, xmin, ymax, xmin, ymin)) {
			DynamicRect->position->x = KinematicRect->position->x - DynamicRect->position->w - 0.01f;
			if (DynamicRect->velocity.x > 0) {
				DynamicRect->velocity.x = 0;
			}
			DynamicRect->sag = 1;
		}
		else if (SKR_IntersectRectLine(DynamicRect->position, xmin, ymin, xmax, ymin) && SKR_IntersectRectLine(DynamicRect->position, xmax, ymin, xmax, ymax) && !SKR_IntersectRectLine(DynamicRect->position, xmax, ymax, xmin, ymax) && !SKR_IntersectRectLine(DynamicRect->position, xmin, ymax, xmin, ymin)) {
			if ((KinematicRect->position->x + KinematicRect->position->w - DynamicRect->position->x) >= ((DynamicRect->position->y + DynamicRect->position->h - KinematicRect->position->y))) {
				DynamicRect->position->y = KinematicRect->position->y - DynamicRect->position->h - 0.01f;
				if (DynamicRect->velocity.y > 0) {
					DynamicRect->velocity.y = 0;
				}
				DynamicRect->alt = 1;
				if (gametype == SKR_SIDESCROLLER) {
					DynamicRect->isonground = 1;
					DynamicRect->position->y += 0.02f;
					if (KinematicRect->velocity.y > 0 && KinematicRect->velocity.y <= gravity) {
						DynamicRect->yk = KinematicRect->velocity.y;
					}
					DynamicRect->xk = KinematicRect->velocity.x;
				}
			}
			else {
				DynamicRect->position->x = KinematicRect->position->x + KinematicRect->position->w + 0.01f;
				if (DynamicRect->velocity.x < 0) {
					DynamicRect->velocity.x = 0;
				}
				DynamicRect->sol = 1;
			}
		}
		else if (!SKR_IntersectRectLine(DynamicRect->position, xmin, ymin, xmax, ymin) && SKR_IntersectRectLine(DynamicRect->position, xmax, ymin, xmax, ymax) && SKR_IntersectRectLine(DynamicRect->position, xmax, ymax, xmin, ymax) && !SKR_IntersectRectLine(DynamicRect->position, xmin, ymax, xmin, ymin)) {
			if ((KinematicRect->position->x + KinematicRect->position->w - DynamicRect->position->x) >= ((KinematicRect->position->y + KinematicRect->position->h - DynamicRect->position->y) + DynamicRect->velocity.y)) {
				DynamicRect->position->y = KinematicRect->position->y + KinematicRect->position->h + 0.01f;
				if (DynamicRect->velocity.y < 0) {
					DynamicRect->velocity.y = 0;
				}
				DynamicRect->ust = 1;
			}
			else {
				DynamicRect->position->x = KinematicRect->position->x + KinematicRect->position->w + 0.01f;
				if (DynamicRect->velocity.x < 0) {
					DynamicRect->velocity.x = 0;
				}
				DynamicRect->sol = 1;
			}
		}
		else if (!SKR_IntersectRectLine(DynamicRect->position, xmin, ymin, xmax, ymin) && !SKR_IntersectRectLine(DynamicRect->position, xmax, ymin, xmax, ymax) && SKR_IntersectRectLine(DynamicRect->position, xmax, ymax, xmin, ymax) && SKR_IntersectRectLine(DynamicRect->position, xmin, ymax, xmin, ymin)) {
			if ((DynamicRect->position->x + DynamicRect->position->w - KinematicRect->position->x) >= ((KinematicRect->position->y + KinematicRect->position->h - DynamicRect->position->y) + DynamicRect->velocity.y)) {
				DynamicRect->position->y = KinematicRect->position->y + KinematicRect->position->h + 0.01f;
				if (DynamicRect->velocity.y < 0) {
					DynamicRect->velocity.y = 0;
				}
				DynamicRect->ust = 1;
			}
			else {
				DynamicRect->position->x = KinematicRect->position->x - DynamicRect->position->w - 0.01f;
				if (DynamicRect->velocity.x > 0) {
					DynamicRect->velocity.x = 0;
				}
				DynamicRect->sag = 1;
			}
		}
		else if (SKR_IntersectRectLine(DynamicRect->position, xmin, ymin, xmax, ymin) && !SKR_IntersectRectLine(DynamicRect->position, xmax, ymin, xmax, ymax) && !SKR_IntersectRectLine(DynamicRect->position, xmax, ymax, xmin, ymax) && SKR_IntersectRectLine(DynamicRect->position, xmin, ymax, xmin, ymin)) {
			if ((DynamicRect->position->x + DynamicRect->position->w - KinematicRect->position->x) >= ((DynamicRect->position->y + DynamicRect->position->h - KinematicRect->position->y))) {
				DynamicRect->position->y = KinematicRect->position->y - DynamicRect->position->h - 0.01f;
				if (DynamicRect->velocity.y > 0) {
					DynamicRect->velocity.y = 0;
				}
				DynamicRect->alt = 1;
				if (gametype == SKR_SIDESCROLLER) {
					DynamicRect->isonground = 1;
					DynamicRect->position->y += 0.02f;
					if (KinematicRect->velocity.y > 0 && KinematicRect->velocity.y <= gravity) {
						DynamicRect->yk = KinematicRect->velocity.y;
					}
					DynamicRect->xk = KinematicRect->velocity.x;
				}
			}
			else {
				DynamicRect->position->x = KinematicRect->position->x - DynamicRect->position->w - 0.01f;
				if (DynamicRect->velocity.x > 0) {
					DynamicRect->velocity.x = 0;
				}
				DynamicRect->sag = 1;
			}
		}
		else if (!SKR_IntersectRectLine(DynamicRect->position, xmin, ymin, xmax, ymin) && SKR_IntersectRectLine(DynamicRect->position, xmax, ymin, xmax, ymax) && SKR_IntersectRectLine(DynamicRect->position, xmax, ymax, xmin, ymax) && SKR_IntersectRectLine(DynamicRect->position, xmin, ymax, xmin, ymin)) {
			DynamicRect->position->y = KinematicRect->position->y + KinematicRect->position->h + 0.01f;
			if (DynamicRect->velocity.y < 0) {
				DynamicRect->velocity.y = 0;
			}
			DynamicRect->ust = 1;
		}
		else if (SKR_IntersectRectLine(DynamicRect->position, xmin, ymin, xmax, ymin) && !SKR_IntersectRectLine(DynamicRect->position, xmax, ymin, xmax, ymax) && SKR_IntersectRectLine(DynamicRect->position, xmax, ymax, xmin, ymax) && SKR_IntersectRectLine(DynamicRect->position, xmin, ymax, xmin, ymin)) {
			DynamicRect->position->x = KinematicRect->position->x - DynamicRect->position->w - 0.01f;
			if (DynamicRect->velocity.x > 0) {
				DynamicRect->velocity.x = 0;
			}
			DynamicRect->sag = 1;
		}
		else if (SKR_IntersectRectLine(DynamicRect->position, xmin, ymin, xmax, ymin) && SKR_IntersectRectLine(DynamicRect->position, xmax, ymin, xmax, ymax) && !SKR_IntersectRectLine(DynamicRect->position, xmax, ymax, xmin, ymax) && SKR_IntersectRectLine(DynamicRect->position, xmin, ymax, xmin, ymin)) {
			DynamicRect->position->y = KinematicRect->position->y - DynamicRect->position->h - 0.01f;
			if (DynamicRect->velocity.y > 0) {
				DynamicRect->velocity.y = 0;
			}
			DynamicRect->alt = 1;
			if (gametype == SKR_SIDESCROLLER) {
				DynamicRect->isonground = 1;
				DynamicRect->position->y += 0.02f;
				if (KinematicRect->velocity.y > 0 && KinematicRect->velocity.y <= gravity) {
					DynamicRect->yk = KinematicRect->velocity.y;
				}
				DynamicRect->xk = KinematicRect->velocity.x;
			}
		}
		else if (SKR_IntersectRectLine(DynamicRect->position, xmin, ymin, xmax, ymin) && SKR_IntersectRectLine(DynamicRect->position, xmax, ymin, xmax, ymax) && SKR_IntersectRectLine(DynamicRect->position, xmax, ymax, xmin, ymax) && !SKR_IntersectRectLine(DynamicRect->position, xmin, ymax, xmin, ymin)) {
			DynamicRect->position->x = KinematicRect->position->x + KinematicRect->position->w + 0.01f;
			if (DynamicRect->velocity.x < 0) {
				DynamicRect->velocity.x = 0;
			}
			DynamicRect->sol = 1;
		}
	}
	return;
}

void CollideDynamicDynamic(SKR_DynamicRect* DynamicRect, SKR_DynamicRect* DynamicRect2) {
	if (DynamicRect != DynamicRect2 && SKR_IntersectRectRect(DynamicRect->position, DynamicRect2->position)) {
		*xmin = DynamicRect2->position->x;
		*ymin = DynamicRect2->position->y;
		*xmax = DynamicRect2->position->x + DynamicRect2->position->w;
		*ymax = DynamicRect2->position->y + DynamicRect2->position->h;
		if (SKR_IntersectRectLine(DynamicRect->position, xmin, ymin, xmax, ymin) && !SKR_IntersectRectLine(DynamicRect->position, xmax, ymin, xmax, ymax) && !SKR_IntersectRectLine(DynamicRect->position, xmax, ymax, xmin, ymax) && !SKR_IntersectRectLine(DynamicRect->position, xmin, ymax, xmin, ymin)) {
			DynamicRect->position->y = DynamicRect2->position->y - DynamicRect->position->h;
			if (DynamicRect2->alt == 1) {
				if (DynamicRect->velocity.y > 0) {
					DynamicRect->velocity.y = 0;
				}
				DynamicRect->isonground = 1;
				DynamicRect->alt = 1;
			}
			else if (DynamicRect->velocity.y > 0) {
				DynamicRect->velocity.y = ((DynamicRect->velocity.y * DynamicRect->mass) + (DynamicRect2->velocity.y * DynamicRect2->mass)) / (DynamicRect->mass + DynamicRect2->mass);
				DynamicRect2->velocity.y = DynamicRect->velocity.y;
			}
		}
		else if (!SKR_IntersectRectLine(DynamicRect->position, xmin, ymin, xmax, ymin) && SKR_IntersectRectLine(DynamicRect->position, xmax, ymin, xmax, ymax) && !SKR_IntersectRectLine(DynamicRect->position, xmax, ymax, xmin, ymax) && !SKR_IntersectRectLine(DynamicRect->position, xmin, ymax, xmin, ymin)) {
			DynamicRect->position->x = DynamicRect2->position->x + DynamicRect2->position->w;
			if (DynamicRect2->sol == 1) {
				if (DynamicRect->velocity.x < 0) {
					DynamicRect->velocity.x = 0;
				}
				DynamicRect->sol = 1;
			}
			else if (DynamicRect->velocity.x < 0) {
				DynamicRect->velocity.x = ((DynamicRect->velocity.x * DynamicRect->mass) + (DynamicRect2->velocity.x * DynamicRect2->mass)) / (DynamicRect->mass + DynamicRect2->mass);
				DynamicRect2->velocity.x = DynamicRect->velocity.x;
			}
		}
		else if (!SKR_IntersectRectLine(DynamicRect->position, xmin, ymin, xmax, ymin) && !SKR_IntersectRectLine(DynamicRect->position, xmax, ymin, xmax, ymax) && SKR_IntersectRectLine(DynamicRect->position, xmax, ymax, xmin, ymax) && !SKR_IntersectRectLine(DynamicRect->position, xmin, ymax, xmin, ymin)) {
			DynamicRect->position->y = DynamicRect2->position->y + DynamicRect2->position->h;
			if (DynamicRect2->ust == 1) {
				if (DynamicRect->velocity.y < 0) {
					DynamicRect->velocity.y = 0;
				}
				DynamicRect->ust = 1;
			}
			else if (DynamicRect->velocity.y < 0) {
				DynamicRect->velocity.y = ((DynamicRect->velocity.y * DynamicRect->mass) + (DynamicRect2->velocity.y * DynamicRect2->mass)) / (DynamicRect->mass + DynamicRect2->mass);
				DynamicRect2->velocity.y = DynamicRect->velocity.y;
			}
		}
		else if (!SKR_IntersectRectLine(DynamicRect->position, xmin, ymin, xmax, ymin) && !SKR_IntersectRectLine(DynamicRect->position, xmax, ymin, xmax, ymax) && !SKR_IntersectRectLine(DynamicRect->position, xmax, ymax, xmin, ymax) && SKR_IntersectRectLine(DynamicRect->position, xmin, ymax, xmin, ymin)) {
			DynamicRect->position->x = DynamicRect2->position->x - DynamicRect->position->w;
			if (DynamicRect2->sag == 1) {
				if (DynamicRect->velocity.x > 0) {
					DynamicRect->velocity.x = 0;
				}
				DynamicRect->sag = 1;
			}
			else if (DynamicRect->velocity.x > 0) {
				DynamicRect->velocity.x = ((DynamicRect->velocity.x * DynamicRect->mass) + (DynamicRect2->velocity.x * DynamicRect2->mass)) / (DynamicRect->mass + DynamicRect2->mass);
				DynamicRect2->velocity.x = DynamicRect->velocity.x;
			}
		}
		else if (SKR_IntersectRectLine(DynamicRect->position, xmin, ymin, xmax, ymin) && SKR_IntersectRectLine(DynamicRect->position, xmax, ymin, xmax, ymax) && !SKR_IntersectRectLine(DynamicRect->position, xmax, ymax, xmin, ymax) && !SKR_IntersectRectLine(DynamicRect->position, xmin, ymax, xmin, ymin)) {
			if ((DynamicRect2->position->x + DynamicRect2->position->w - DynamicRect->position->x) >= ((DynamicRect->position->y + DynamicRect->position->h - DynamicRect2->position->y))) {
				DynamicRect->position->y = DynamicRect2->position->y - DynamicRect->position->h;
				if (DynamicRect2->alt == 1) {
					if (DynamicRect->velocity.y > 0) {
						DynamicRect->velocity.y = 0;
					}
					DynamicRect->isonground = 1;
					DynamicRect->alt = 1;
				}
				else if (DynamicRect->velocity.y > 0) {
					DynamicRect->velocity.y = ((DynamicRect->velocity.y * DynamicRect->mass) + (DynamicRect2->velocity.y * DynamicRect2->mass)) / (DynamicRect->mass + DynamicRect2->mass);
					DynamicRect2->velocity.y = DynamicRect->velocity.y;
				}
			}
			else {
				DynamicRect->position->x = DynamicRect2->position->x + DynamicRect2->position->w;
				if (DynamicRect2->sol == 1) {
					if (DynamicRect->velocity.x < 0) {
						DynamicRect->velocity.x = 0;
					}
					DynamicRect->sol = 1;
				}
				else if (DynamicRect->velocity.x < 0) {
					DynamicRect->velocity.x = ((DynamicRect->velocity.x * DynamicRect->mass) + (DynamicRect2->velocity.x * DynamicRect2->mass)) / (DynamicRect->mass + DynamicRect2->mass);
					DynamicRect2->velocity.x = DynamicRect->velocity.x;
				}
			}
		}
		else if (!SKR_IntersectRectLine(DynamicRect->position, xmin, ymin, xmax, ymin) && SKR_IntersectRectLine(DynamicRect->position, xmax, ymin, xmax, ymax) && SKR_IntersectRectLine(DynamicRect->position, xmax, ymax, xmin, ymax) && !SKR_IntersectRectLine(DynamicRect->position, xmin, ymax, xmin, ymin)) {
			if ((DynamicRect2->position->x + DynamicRect2->position->w - DynamicRect->position->x) >= ((DynamicRect2->position->y + DynamicRect2->position->h - DynamicRect->position->y) + DynamicRect->velocity.y)) {
				DynamicRect->position->y = DynamicRect2->position->y + DynamicRect2->position->h;
				if (DynamicRect2->ust == 1) {
					if (DynamicRect->velocity.y < 0) {
						DynamicRect->velocity.y = 0;
					}
					DynamicRect->ust = 1;
				}
				else if (DynamicRect->velocity.y < 0) {
					DynamicRect->velocity.y = ((DynamicRect->velocity.y * DynamicRect->mass) + (DynamicRect2->velocity.y * DynamicRect2->mass)) / (DynamicRect->mass + DynamicRect2->mass);
					DynamicRect2->velocity.y = DynamicRect->velocity.y;
				}
			}
			else {
				DynamicRect->position->x = DynamicRect2->position->x + DynamicRect2->position->w;
				if (DynamicRect2->sol == 1) {
					if (DynamicRect->velocity.x < 0) {
						DynamicRect->velocity.x = 0;
					}
					DynamicRect->sol = 1;
				}
				else if (DynamicRect->velocity.x < 0) {
					DynamicRect->velocity.x = ((DynamicRect->velocity.x * DynamicRect->mass) + (DynamicRect2->velocity.x * DynamicRect2->mass)) / (DynamicRect->mass + DynamicRect2->mass);
					DynamicRect2->velocity.x = DynamicRect->velocity.x;
				}
			}
		}
		else if (!SKR_IntersectRectLine(DynamicRect->position, xmin, ymin, xmax, ymin) && !SKR_IntersectRectLine(DynamicRect->position, xmax, ymin, xmax, ymax) && SKR_IntersectRectLine(DynamicRect->position, xmax, ymax, xmin, ymax) && SKR_IntersectRectLine(DynamicRect->position, xmin, ymax, xmin, ymin)) {
			if ((DynamicRect->position->x + DynamicRect->position->w - DynamicRect2->position->x) >= ((DynamicRect2->position->y + DynamicRect2->position->h - DynamicRect->position->y) + DynamicRect->velocity.y)) {
				DynamicRect->position->y = DynamicRect2->position->y + DynamicRect2->position->h;
				if (DynamicRect2->ust == 1) {
					if (DynamicRect->velocity.y < 0) {
						DynamicRect->velocity.y = 0;
					}
					DynamicRect->ust = 1;
				}
				else if (DynamicRect->velocity.y < 0) {
					DynamicRect->velocity.y = ((DynamicRect->velocity.y * DynamicRect->mass) + (DynamicRect2->velocity.y * DynamicRect2->mass)) / (DynamicRect->mass + DynamicRect2->mass);
					DynamicRect2->velocity.y = DynamicRect->velocity.y;
				}
			}
			else {
				DynamicRect->position->x = DynamicRect2->position->x - DynamicRect->position->w;
				if (DynamicRect2->sag == 1) {
					if (DynamicRect->velocity.x > 0) {
						DynamicRect->velocity.x = 0;
					}
					DynamicRect->sag = 1;
				}
				else if (DynamicRect->velocity.x > 0) {
					DynamicRect->velocity.x = ((DynamicRect->velocity.x * DynamicRect->mass) + (DynamicRect2->velocity.x * DynamicRect2->mass)) / (DynamicRect->mass + DynamicRect2->mass);
					DynamicRect2->velocity.x = DynamicRect->velocity.x;
				}
			}
		}
		else if (SKR_IntersectRectLine(DynamicRect->position, xmin, ymin, xmax, ymin) && !SKR_IntersectRectLine(DynamicRect->position, xmax, ymin, xmax, ymax) && !SKR_IntersectRectLine(DynamicRect->position, xmax, ymax, xmin, ymax) && SKR_IntersectRectLine(DynamicRect->position, xmin, ymax, xmin, ymin)) {
			if ((DynamicRect->position->x + DynamicRect->position->w - DynamicRect2->position->x) >= ((DynamicRect->position->y + DynamicRect->position->h - DynamicRect2->position->y))) {
				DynamicRect->position->y = DynamicRect2->position->y - DynamicRect->position->h;
				if (DynamicRect2->alt == 1) {
					if (DynamicRect->velocity.y > 0) {
						DynamicRect->velocity.y = 0;
					}
					DynamicRect->isonground = 1;
					DynamicRect->alt = 1;
				}
				else if (DynamicRect->velocity.y > 0) {
					DynamicRect->velocity.y = ((DynamicRect->velocity.y * DynamicRect->mass) + (DynamicRect2->velocity.y * DynamicRect2->mass)) / (DynamicRect->mass + DynamicRect2->mass);
					DynamicRect2->velocity.y = DynamicRect->velocity.y;
				}
			}
			else {
				DynamicRect->position->x = DynamicRect2->position->x - DynamicRect->position->w;
				if (DynamicRect2->sag == 1) {
					if (DynamicRect->velocity.x > 0) {
						DynamicRect->velocity.x = 0;
					}
					DynamicRect->sag = 1;
				}
				else if (DynamicRect->velocity.x > 0) {
					DynamicRect->velocity.x = ((DynamicRect->velocity.x * DynamicRect->mass) + (DynamicRect2->velocity.x * DynamicRect2->mass)) / (DynamicRect->mass + DynamicRect2->mass);
					DynamicRect2->velocity.x = DynamicRect->velocity.x;
				}
			}
		}
		else if (!SKR_IntersectRectLine(DynamicRect->position, xmin, ymin, xmax, ymin) && SKR_IntersectRectLine(DynamicRect->position, xmax, ymin, xmax, ymax) && SKR_IntersectRectLine(DynamicRect->position, xmax, ymax, xmin, ymax) && SKR_IntersectRectLine(DynamicRect->position, xmin, ymax, xmin, ymin)) {
			DynamicRect->position->y = DynamicRect2->position->y + DynamicRect2->position->h;
			if (DynamicRect2->ust == 1) {
				if (DynamicRect->velocity.y < 0) {
					DynamicRect->velocity.y = 0;
				}
				DynamicRect->ust = 1;
			}
			else if (DynamicRect->velocity.y < 0) {
				DynamicRect->velocity.y = ((DynamicRect->velocity.y * DynamicRect->mass) + (DynamicRect2->velocity.y * DynamicRect2->mass)) / (DynamicRect->mass + DynamicRect2->mass);
				DynamicRect2->velocity.y = DynamicRect->velocity.y;
			}
		}
		else if (SKR_IntersectRectLine(DynamicRect->position, xmin, ymin, xmax, ymin) && !SKR_IntersectRectLine(DynamicRect->position, xmax, ymin, xmax, ymax) && SKR_IntersectRectLine(DynamicRect->position, xmax, ymax, xmin, ymax) && SKR_IntersectRectLine(DynamicRect->position, xmin, ymax, xmin, ymin)) {
			DynamicRect->position->x = DynamicRect2->position->x - DynamicRect->position->w;
			if (DynamicRect2->sag == 1) {
				if (DynamicRect->velocity.x > 0) {
					DynamicRect->velocity.x = 0;
				}
				DynamicRect->sag = 1;
			}
			else if (DynamicRect->velocity.x > 0) {
				DynamicRect->velocity.x = ((DynamicRect->velocity.x * DynamicRect->mass) + (DynamicRect2->velocity.x * DynamicRect2->mass)) / (DynamicRect->mass + DynamicRect2->mass);
				DynamicRect2->velocity.x = DynamicRect->velocity.x;
			}
		}
		else if (SKR_IntersectRectLine(DynamicRect->position, xmin, ymin, xmax, ymin) && SKR_IntersectRectLine(DynamicRect->position, xmax, ymin, xmax, ymax) && !SKR_IntersectRectLine(DynamicRect->position, xmax, ymax, xmin, ymax) && SKR_IntersectRectLine(DynamicRect->position, xmin, ymax, xmin, ymin)) {
			DynamicRect->position->y = DynamicRect2->position->y - DynamicRect->position->h;
			if (DynamicRect2->alt == 1) {
				if (DynamicRect->velocity.y > 0) {
					DynamicRect->velocity.y = 0;
				}
				DynamicRect->isonground = 1;
				DynamicRect->alt = 1;
			}
			else if (DynamicRect->velocity.y > 0) {
				DynamicRect->velocity.y = ((DynamicRect->velocity.y * DynamicRect->mass) + (DynamicRect2->velocity.y * DynamicRect2->mass)) / (DynamicRect->mass + DynamicRect2->mass);
				DynamicRect2->velocity.y = DynamicRect->velocity.y;
			}
		}
		else if (SKR_IntersectRectLine(DynamicRect->position, xmin, ymin, xmax, ymin) && SKR_IntersectRectLine(DynamicRect->position, xmax, ymin, xmax, ymax) && SKR_IntersectRectLine(DynamicRect->position, xmax, ymax, xmin, ymax) && !SKR_IntersectRectLine(DynamicRect->position, xmin, ymax, xmin, ymin)) {
			DynamicRect->position->x = DynamicRect2->position->x + DynamicRect2->position->w;
			if (DynamicRect2->sol == 1) {
				if (DynamicRect->velocity.x < 0) {
					DynamicRect->velocity.x = 0;
				}
				DynamicRect->sol = 1;
			}
			else if (DynamicRect->velocity.x < 0) {
				DynamicRect->velocity.x = ((DynamicRect->velocity.x * DynamicRect->mass) + (DynamicRect2->velocity.x * DynamicRect2->mass)) / (DynamicRect->mass + DynamicRect2->mass);
				DynamicRect2->velocity.x = DynamicRect->velocity.x;
			}
		}
	}
	return;
}

SKR_RectWorld* SKR_CreateRectWorld(float Gravity, float AirFrictionCoefficient, SKR_GAMETYPE GameType) {
	if (xmax == NULL) {
		xmax = new float;
	}
	if (xmin == NULL) {
		xmin = new float;
	}
	if (ymax == NULL) {
		ymax = new float;
	}
	if (ymin == NULL) {
		ymin = new float;
	}
	SKR_RectWorld* world = new SKR_RectWorld;
	world->gravity = Gravity;
	world->airfriction = AirFrictionCoefficient;
	world->StaticRectList = NULL;
	world->KinematicRectList = NULL;
	world->DynamicRectList = NULL;
	world->gametype = GameType;
	if (GameType == SKR_ISOMETRIC) {
		world->gravity = 0;
	}
	return world;
}

void SKR_SetGravity(SKR_RectWorld* World, float Gravity) {
	if (World != NULL) {
		World->gravity = Gravity;
	}
	return;
}

float SKR_GetGravity(SKR_RectWorld* World) {
	if (World != NULL) {
		return World->gravity;
	}
	return 0;
}

void SKR_SetAirFriction(SKR_RectWorld* World, float AirFrictionCoefficient) {
	if (World != NULL) {
		World->airfriction = AirFrictionCoefficient;
	}
	return;
}

float SKR_GetAirFriction(SKR_RectWorld* World) {
	if (World != NULL) {
		return World->airfriction;
	}
	return 0;
}

void SKR_DestroyRectWorld(SKR_RectWorld* World) {
	SKR_StaticRect* tmp = World->StaticRectList;
	while (World->StaticRectList != NULL) {
		tmp = World->StaticRectList;
		World->StaticRectList = World->StaticRectList->sonraki;
		free(tmp);
		tmp = NULL;
	}
	SKR_KinematicRect* tmp2 = World->KinematicRectList;
	while (World->KinematicRectList != NULL) {
		tmp2 = World->KinematicRectList;
		World->KinematicRectList = World->KinematicRectList->sonraki;
		free(tmp2);
		tmp2 = NULL;
	}
	SKR_DynamicRect* tmp3 = World->DynamicRectList;
	while (World->DynamicRectList != NULL) {
		tmp3 = World->DynamicRectList;
		World->DynamicRectList = World->DynamicRectList->sonraki;
		free(tmp3);
		tmp3 = NULL;
	}
	free(World);
	World = NULL;
	return;
}

SKR_StaticRect* SKR_CreateStaticRect(SKR_RectWorld* World, SDL_FRect* Position) {
	if (World == NULL) {
		return NULL;
	}
	SKR_StaticRect* yeni = new SKR_StaticRect;
	yeni->position = Position;
	yeni->sonraki = World->StaticRectList;
	World->StaticRectList = yeni;
	return yeni;
}

void SKR_DestroyStaticRect(SKR_RectWorld* World, SKR_StaticRect* StaticRect) {
	if (World->StaticRectList == StaticRect) {
		World->StaticRectList = World->StaticRectList->sonraki;
		free(StaticRect);
		StaticRect = NULL;
		return;
	}
	SKR_StaticRect* tmp = World->StaticRectList;
	SKR_StaticRect* tmp2 = NULL;
	while (tmp != NULL) {
		if (tmp == StaticRect) {
			tmp2->sonraki = tmp->sonraki;
			free(StaticRect);
			StaticRect = NULL;
			return;
		}
		tmp2 = tmp;
		tmp = tmp->sonraki;
	}
	return;
}

SDL_FRect* SKR_GetPositionStaticRect(SKR_StaticRect* StaticRect) {
	if (StaticRect != NULL) {
		return StaticRect->position;
	}
	else {
		return NULL;
	}
}

int SKR_GetStaticRectNumber(SKR_RectWorld* World) {
	if (World == NULL) {
		return 0;
	}
	int size = 0;
	SKR_StaticRect* tmp = World->StaticRectList;
	while (tmp != NULL) {
		size++;
		tmp = tmp->sonraki;
	}
	return size;
}

SKR_KinematicRect* SKR_CreateKinematicRect(SKR_RectWorld* World, SDL_FRect* Position) {
	if (World == NULL) {
		return NULL;
	}
	SKR_KinematicRect* yeni = new SKR_KinematicRect;
	yeni->position = Position;
	yeni->sonraki = World->KinematicRectList;
	yeni->velocity.x = 0;
	yeni->velocity.y = 0;
	yeni->boolean = -1;
	World->KinematicRectList = yeni;
	return yeni;
}

void SKR_DestroyKinematicRect(SKR_RectWorld* World, SKR_KinematicRect* KinematicRect) {
	if (World->KinematicRectList == KinematicRect) {
		World->KinematicRectList = World->KinematicRectList->sonraki;
		free(KinematicRect);
		KinematicRect = NULL;
		return;
	}
	SKR_KinematicRect* tmp = World->KinematicRectList;
	SKR_KinematicRect* tmp2 = NULL;
	while (tmp != NULL) {
		if (tmp == KinematicRect) {
			tmp2->sonraki = tmp->sonraki;
			free(KinematicRect);
			KinematicRect = NULL;
			return;
		}
		tmp2 = tmp;
		tmp = tmp->sonraki;
	}
	return;
}

SDL_FRect* SKR_GetPositionKinematicRect(SKR_KinematicRect* KinematicRect) {
	if (KinematicRect != NULL) {
		return KinematicRect->position;
	}
	else {
		return NULL;
	}
}

int SKR_GetKinematicRectNumber(SKR_RectWorld* World) {
	if (World == NULL) {
		return 0;
	}
	int size = 0;
	SKR_KinematicRect* tmp = World->KinematicRectList;
	while (tmp != NULL) {
		size++;
		tmp = tmp->sonraki;
	}
	return size;
}

void SKR_SetXVelocityKinematicRect(SKR_KinematicRect* KinematicRect, float Xspeed) {
	if (KinematicRect != NULL) {
		KinematicRect->velocity.x = Xspeed;
	}
	return;
}

void SKR_SetYVelocityKinematicRect(SKR_KinematicRect* KinematicRect, float Yspeed) {
	if (KinematicRect != NULL) {
		KinematicRect->velocity.y = Yspeed;
	}
	return;
}

float SKR_GetXVelocityKinematicRect(SKR_KinematicRect* KinematicRect) {
	if (KinematicRect != NULL) {
		return KinematicRect->velocity.x;
	}
	else {
		return 0;
	}
}

float SKR_GetYVelocityKinematicRect(SKR_KinematicRect* KinematicRect) {
	if (KinematicRect != NULL) {
		return KinematicRect->velocity.y;
	}
	else {
		return 0;
	}
}

void SKR_AnimateKinematicRect(SKR_KinematicRect* KinematicRect, float X1, float Y1, float X2, float Y2, float Velocity) {
	Velocity = fabsf(Velocity);
	if (KinematicRect == NULL) {
		return;
	}
	else if (KinematicRect->boolean == -2) {
		return;
	}
	else if (KinematicRect->boolean == -1) {
		KinematicRect->position->x = X1;
		KinematicRect->position->y = Y1;
		KinematicRect->boolean = 0;
	}
	else if (KinematicRect->boolean == 0) {
		KinematicRect->velocity.x = ((X2 - X1) / sqrtf((powf(X2 - X1, 2) + powf(Y2 - Y1, 2)))) * Velocity;
		KinematicRect->velocity.y = ((Y2 - Y1) / sqrtf((powf(X2 - X1, 2) + powf(Y2 - Y1, 2)))) * Velocity;
		if (X1 > X2) {
			if (KinematicRect->position->x <= X2) {
				KinematicRect->position->x = X2;
				KinematicRect->position->y = Y2;
				KinematicRect->boolean = 1;
			}
		}
		else if (X1 < X2) {
			if (KinematicRect->position->x >= X2) {
				KinematicRect->position->x = X2;
				KinematicRect->position->y = Y2;
				KinematicRect->boolean = 1;
			}
		}
		else if (Y1 > Y2) {
			if (KinematicRect->position->x <= Y2) {
				KinematicRect->position->x = X2;
				KinematicRect->position->y = Y2;
				KinematicRect->boolean = 1;
			}
		}
		else if (Y1 < Y2) {
			if (KinematicRect->position->x >= Y2) {
				KinematicRect->position->x = X2;
				KinematicRect->position->y = Y2;
				KinematicRect->boolean = 1;
			}
		}
		else {
			KinematicRect->velocity.x = 0;
			KinematicRect->velocity.y = 0;
			KinematicRect->boolean = -2;
		}
	}
	else if (KinematicRect->boolean == 1) {
		KinematicRect->velocity.x = ((X1 - X2) / sqrtf((powf(X2 - X1, 2) + powf(Y2 - Y1, 2)))) * Velocity;
		KinematicRect->velocity.y = ((Y1 - Y2) / sqrtf((powf(X2 - X1, 2) + powf(Y2 - Y1, 2)))) * Velocity;
		if (X1 > X2) {
			if (KinematicRect->position->x >= X1) {
				KinematicRect->position->x = X1;
				KinematicRect->position->y = Y1;
				KinematicRect->boolean = 0;
			}
		}
		else if (X1 < X2) {
			if (KinematicRect->position->x <= X1) {
				KinematicRect->position->x = X1;
				KinematicRect->position->y = Y1;
				KinematicRect->boolean = 0;
			}
		}
		else if (Y1 > Y2) {
			if (KinematicRect->position->x >= Y1) {
				KinematicRect->position->x = X1;
				KinematicRect->position->y = Y1;
				KinematicRect->boolean = 0;
			}
		}
		else if (Y1 < Y2) {
			if (KinematicRect->position->x <= Y1) {
				KinematicRect->position->x = X1;
				KinematicRect->position->y = Y1;
				KinematicRect->boolean = 0;
			}
		}
		else {
			KinematicRect->velocity.x = 0;
			KinematicRect->velocity.y = 0;
			KinematicRect->boolean = -2;
		}
	}
	return;
}

void SKR_StopAnimatingKinematicRect(SKR_KinematicRect* KinematicRect) {
	if (KinematicRect == NULL) {
		return;
	}
	KinematicRect->boolean = -1;
	KinematicRect->velocity.x = 0;
	KinematicRect->velocity.y = 0;
	return;
}

SKR_DynamicRect* SKR_CreateDynamicRect(SKR_RectWorld* World, SDL_FRect* Position, float Mass, float FrictionCoefficient, float GravityMultiplier) {
	if (World == NULL) {
		return NULL;
	}
	SKR_DynamicRect* yeni = new SKR_DynamicRect;
	yeni->position = Position;
	yeni->sonraki = World->DynamicRectList;
	yeni->mass = Mass;
	yeni->friction = FrictionCoefficient;
	yeni->velocity.x = 0;
	yeni->velocity.y = 0;
	yeni->force.x = 0;
	yeni->force.y = 0;
	yeni->gravitymultiplier = GravityMultiplier;
	yeni->xk = 0;
	yeni->yk = 0;
	yeni->alt = 0;
	yeni->sol = 0;
	yeni->sag = 0;
	yeni->ust = 0;
	World->DynamicRectList = yeni;
	return yeni;
}

void SKR_DestroyDynamicRect(SKR_RectWorld* World, SKR_DynamicRect* DynamicRect) {
	if (World->DynamicRectList == DynamicRect) {
		World->DynamicRectList = World->DynamicRectList->sonraki;
		free(DynamicRect);
		DynamicRect = NULL;
		return;
	}
	SKR_DynamicRect* tmp = World->DynamicRectList;
	SKR_DynamicRect* tmp2 = NULL;
	while (tmp != NULL) {
		if (tmp == DynamicRect) {
			tmp2->sonraki = tmp->sonraki;
			free(DynamicRect);
			DynamicRect = NULL;
			return;
		}
		tmp2 = tmp;
		tmp = tmp->sonraki;
	}
	return;
}

SDL_FRect* SKR_GetPositionDynamicRect(SKR_DynamicRect* DynamicRect) {
	if (DynamicRect != NULL) {
		return DynamicRect->position;
	}
	else {
		return NULL;
	}
}

int SKR_GetDynamicRectNumber(SKR_RectWorld* World) {
	if (World == NULL) {
		return 0;
	}
	int size = 0;
	SKR_DynamicRect* tmp = World->DynamicRectList;
	while (tmp != NULL) {
		size++;
		tmp = tmp->sonraki;
	}
	return size;
}

void SKR_SetXVelocityDynamicRect(SKR_DynamicRect* DynamicRect, float Xspeed) {
	if (DynamicRect != NULL) {
		DynamicRect->velocity.x = Xspeed;
	}
	return;
}

void SKR_SetYVelocityDynamicRect(SKR_DynamicRect* DynamicRect, float Yspeed) {
	if (DynamicRect != NULL) {
		DynamicRect->velocity.y = Yspeed;
	}
	return;
}

float SKR_GetXVelocityDynamicRect(SKR_DynamicRect* DynamicRect) {
	if (DynamicRect != NULL) {
		return DynamicRect->velocity.x;
	}
	else {
		return 0;
	}
}

float SKR_GetYVelocityDynamicRect(SKR_DynamicRect* DynamicRect) {
	if (DynamicRect != NULL) {
		return DynamicRect->velocity.y;
	}
	else {
		return 0;
	}
}

float SKR_GetMassDynamicRect(SKR_DynamicRect* DynamicRect) {
	if (DynamicRect != NULL) {
		return DynamicRect->mass;
	}
	else {
		return 0;
	}
}

void SKR_SetMassDynamicRect(SKR_DynamicRect* DynamicRect, float Mass) {
	if (DynamicRect != NULL) {
		DynamicRect->mass = Mass;
	}
	return;
}

void SKR_ApplyForceX(SKR_DynamicRect* DynamicRect, float Force) {
	if (DynamicRect != NULL) {
		DynamicRect->force.x = DynamicRect->force.x + Force;
	}
	return;
}

void SKR_ApplyForceY(SKR_DynamicRect* DynamicRect, float Force) {
	if (DynamicRect != NULL) {
		DynamicRect->force.y = DynamicRect->force.y + Force;
	}
	return;
}

void SKR_SetForceX(SKR_DynamicRect* DynamicRect, float Force) {
	if (DynamicRect != NULL) {
		DynamicRect->force.x = Force;
	}
	return;
}

void SKR_SetForceY(SKR_DynamicRect* DynamicRect, float Force) {
	if (DynamicRect != NULL) {
		DynamicRect->force.y = Force;
	}
	return;
}

float SKR_GetForceX(SKR_DynamicRect* DynamicRect) {
	if (DynamicRect != NULL) {
		return DynamicRect->force.x;
	}
	return 0;
}

float SKR_GetForceY(SKR_DynamicRect* DynamicRect) {
	if (DynamicRect != NULL) {
		return DynamicRect->force.y;
	}
	return 0;
}

void SKR_SimulateWorld(SKR_RectWorld* World, float Milliseconds) {
	tmpD = World->DynamicRectList;
	tmpD2 = World->DynamicRectList;
	tmpS = World->StaticRectList;
	tmpK = World->KinematicRectList;
	while (tmpK != NULL) {
		tmpK->position->x = tmpK->position->x + (tmpK->velocity.x * (Milliseconds / 10));
		tmpK->position->y = tmpK->position->y + (tmpK->velocity.y * (Milliseconds / 10));
		tmpK = tmpK->sonraki;
	}
	while (tmpD != NULL) {
		xmax2 = tmpD->velocity.x;
		tmpD->velocity.x = tmpD->velocity.x + (tmpD->force.x / tmpD->mass);
		if (tmpD->velocity.x > 0) {
			tmpD->velocity.x = tmpD->velocity.x - fabsf(xmax2 * World->airfriction * (Milliseconds / 1000));
			if (tmpD->alt == 1 || tmpD->ust == 1) {
				tmpD->velocity.x = tmpD->velocity.x - fabsf(xmax2 * tmpD->friction * (Milliseconds / 1000));
			}
			if (tmpD->velocity.x < 0.01f) {
				tmpD->velocity.x = 0;
			}
		}
		else if (tmpD->velocity.x < 0) {
			tmpD->velocity.x = tmpD->velocity.x + fabsf(xmax2 * World->airfriction * (Milliseconds / 1000));
			if (tmpD->alt == 1 || tmpD->ust == 1) {
				tmpD->velocity.x = tmpD->velocity.x + fabsf(xmax2 * tmpD->friction * (Milliseconds / 1000));
			}
			if (tmpD->velocity.x > -0.01f) {
				tmpD->velocity.x = 0;
			}
		}
		ymax2 = tmpD->velocity.y;
		tmpD->velocity.y = tmpD->velocity.y + (tmpD->force.y / tmpD->mass) + (World->gravity * (tmpD->gravitymultiplier) * (Milliseconds / 1000));
		if (tmpD->velocity.y > 0) {
			tmpD->velocity.y = tmpD->velocity.y - fabsf(ymax2 * World->airfriction * (Milliseconds / 1000));
			if (tmpD->sol == 1 || tmpD->sag == 1) {
				tmpD->velocity.y = tmpD->velocity.y - fabsf(ymax2 * tmpD->friction * (Milliseconds / 1000));
			}
			if (tmpD->velocity.y < 0.01f) {
				tmpD->velocity.y = 0;
			}
		}
		else if (tmpD->velocity.y < 0) {
			tmpD->velocity.y = tmpD->velocity.y + fabsf(ymax2 * World->airfriction * (Milliseconds / 1000));
			if (tmpD->sol == 1 || tmpD->sag == 1) {
				tmpD->velocity.y = tmpD->velocity.y + fabsf(ymax2 * tmpD->friction * (Milliseconds / 1000));
			}
			if (tmpD->velocity.y > -0.01f) {
				tmpD->velocity.y = 0;
			}
		}
		tmpD->force.x = 0;
		tmpD->force.y = 0;
		xmax2 = ((tmpD->velocity.x + tmpD->xk) * (Milliseconds / 10));
		ymax2 = ((tmpD->velocity.y + tmpD->yk) * (Milliseconds / 10));
		xmin2 = max(fabsf(xmax2), fabsf(ymax2));
		loop = (int)xmin2;
		if (loop < 1) {
			loop = 1;
		}
		tmpD->isonground = 0;
		tmpD->xk = 0;
		tmpD->yk = 0;
		tmpD->sol = 0;
		tmpD->sag = 0;
		tmpD->alt = 0;
		tmpD->ust = 0;
		if (xmin2 == 0) {
			tmpS = World->StaticRectList;
			while (tmpS != NULL) {
				CollideDynamicStatic(tmpD, tmpS);
				tmpS = tmpS->sonraki;
			}
			tmpK = World->KinematicRectList;
			while (tmpK != NULL) {
				CollideDynamicKinematic(tmpD, tmpK, (World->gravity * (tmpD->gravitymultiplier)), World->gametype);
				tmpK = tmpK->sonraki;
			}
			tmpD2 = World->DynamicRectList;
			while (tmpD2 != NULL) {
				CollideDynamicDynamic(tmpD, tmpD2);
				tmpD2 = tmpD2->sonraki;
			}
		}
		else {
			for (i = 0; i < loop; i++) {
				tmpD->position->x = tmpD->position->x + (xmax2 / loop);
				tmpD->position->y = tmpD->position->y + (ymax2 / loop);
				tmpS = World->StaticRectList;
				while (tmpS != NULL) {
					CollideDynamicStatic(tmpD, tmpS);
					tmpS = tmpS->sonraki;
				}
				tmpK = World->KinematicRectList;
				while (tmpK != NULL) {
					CollideDynamicKinematic(tmpD, tmpK, (World->gravity * (tmpD->gravitymultiplier)), World->gametype);
					tmpK = tmpK->sonraki;
				}
				tmpD2 = World->DynamicRectList;
				while (tmpD2 != NULL) {
					CollideDynamicDynamic(tmpD, tmpD2);
					tmpD2 = tmpD2->sonraki;
				}
			}
		}
		if (World->gametype == SKR_ISOMETRIC) {
			tmpD->isonground = 0;
		}
		tmpD = tmpD->sonraki;
	}
	return;
}

int SKR_IsOnground(SKR_DynamicRect* DynamicRect) {
	if (DynamicRect != NULL) {
		return DynamicRect->isonground;
	}
	else {
		return 0;
	}
}

float SKR_GetFriction(SKR_DynamicRect* DynamicRect) {
	if (DynamicRect != NULL) {
		return DynamicRect->friction;
	}
	return 0;
}

void SKR_SetFriction(SKR_DynamicRect* DynamicRect, float FrictionCoefficient) {
	if (DynamicRect != NULL) {
		DynamicRect->friction = FrictionCoefficient;
	}
	return;
}

float SKR_GetGravityMultiplier(SKR_DynamicRect* DynamicRect) {
	if (DynamicRect != NULL) {
		return DynamicRect->gravitymultiplier;
	}
	return 0;
}

void SKR_SetGravityMultiplier(SKR_DynamicRect* DynamicRect, float GravityMultiplier) {
	if (DynamicRect != NULL) {
		DynamicRect->gravitymultiplier = GravityMultiplier;
	}
	return;
}
#endif
// ############### SKR_Physics.cpp End ###############

namespace AST {
	SDL_Window* win;
	SDL_Renderer* ren;
	SDL_Point Mouse;
	int grid = 1;
	int FPS = 60;
	bool loop;
	bool isFullscreen = false;
	std::unordered_map<int, bool> keys;
	int code;

#ifdef AST_PHYSICS
    SKR_RectWorld * world;
#endif

	Scene::Scene() {
		AST::loop = true;
	}

	void Scene::loop() {
		
	}

	void Scene::event(SDL_Event& event) {
		if (AST::keys[SDL_QUIT]) AST::loop = false;
	}

	Scene::~Scene() {
#ifdef AST_TEXTURE
		SpriteManager::free();
#endif
	}
#ifdef AST_PHYSICS
	void Init(std::string title, SDL_Rect rect, float WorldGravity, float WorldAirFrictionCoefficient, SKR_GAMETYPE WorldGameType) {
#else
	void Init(std::string title, SDL_Rect rect) {
#endif
	    SDL_Init(SDL_INIT_EVERYTHING);

	    if(rect.w == -1 && rect.h == -1) {

	    	win = SDL_CreateWindow(title.c_str(), 
	    		SDL_WINDOWPOS_UNDEFINED, 
	    		SDL_WINDOWPOS_UNDEFINED, 
	    		rect.w, 
	    		rect.h, 
	    		SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED);

	    	SDL_SetWindowResizable(win, SDL_FALSE);
	    } else {

	    	win = SDL_CreateWindow(title.c_str(), 
	    		rect.x == -1 ? SDL_WINDOWPOS_CENTERED : rect.x, 
	    		rect.y == -1 ? SDL_WINDOWPOS_CENTERED : rect.y, 
	    		rect.w, 
	    		rect.h, 
	    		0);
	    }

	    ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_PRESENTVSYNC);

	    SDL_RenderSetLogicalSize(ren, 1920, 1080);

	    float scaleX, scaleY;
	    SDL_Rect windowRect;
	    SDL_GetWindowSize(win, &windowRect.w, &windowRect.h);

	    scaleX = static_cast<float>(windowRect.w) / 1920;
	    scaleY = static_cast<float>(windowRect.h) / 1080;

	    SDL_RenderSetScale(ren, scaleX, scaleY);
#ifdef AST_PHYSICS
	    world = SKR_CreateRectWorld(WorldGravity, WorldAirFrictionCoefficient, WorldGameType);
#endif
	}
	

	void Render(Scene & scene) {
		while (AST::loop) {
			Uint32 frameStart = SDL_GetTicks();

			//Event Handling
			SDL_Event event;
			keys.clear();
			while (SDL_PollEvent(&event)) {
				SDL_GetMouseState(&Mouse.x, &Mouse.y);

				Mouse.x /= grid;
				Mouse.y /= grid;
				Mouse.x *= grid;
				Mouse.y *= grid;

				keys[event.type] = true;
				if (event.type == SDL_MOUSEBUTTONDOWN) keys[event.button.button] = true;
				if (event.type == SDL_KEYDOWN) keys[event.key.keysym.sym] = true;
				
				scene.event(event);
			}

#ifdef AST_PHYSICS
			//Physics
			SKR_SimulateWorld(world, 1000/FPS);
#endif
			//Rendering
			SDL_RenderClear(ren);
			scene.loop();
			SDL_RenderPresent(ren);

			Uint32 frameTime = SDL_GetTicks() - frameStart;
			if (frameTime < 1000/FPS) SDL_Delay(1000/FPS - frameTime);
		}
	}

	void Quit() {
#ifdef AST_PHYSICS
		SKR_DestroyRectWorld(world);
#endif
		SDL_DestroyWindow(win);
		SDL_DestroyRenderer(ren);
		SDL_Quit();
	}

#ifdef AST_PHYSICS
	void Rect::Physics(float mass, float friction, float gravity) {

		if(mass != 0.0 && friction != 0.0 && gravity != 0.0)
			DynamicRect = SKR_CreateDynamicRect(world, this, mass, friction, gravity);
		else
			StaticRect = SKR_CreateStaticRect(world, this);
		
	}
#endif
#ifdef AST_TEXTURE
	Rect::Rect(SDL_FRect rect, std::string texture) {
		Init(rect);
		this->texture = texture;
		this->type = 0;
	}
#endif
	Rect::Rect(SDL_FRect rect, SDL_Color color) {
		Init(rect);
		start = color;
		this->type = 1;
	}

	Rect::Rect(SDL_FRect rect, SDL_Color start, SDL_Color end) {
		Init(rect);
		this->start = start;
		this->end = end;
		this->type = 2;
	}

	void Rect::Init(SDL_FRect rect) {
		x = rect.x;
		y = rect.y;

		if(rect.w == -1) {
			w = 1920;
		} else w = rect.w;
		if(rect.w == -1) {
			h = 1080;
		} else h = rect.h;
		#ifndef AST_PHYSICS
			angle = 0.0;
		#endif
	}
	Rect::~Rect() {
// #ifdef AST_PHYSICS
// 		SKR_DestroyStaticRect(world, this->StaticRect);
// 		SKR_DestroyDynamicRect(world, this->DynamicRect);
// #endif
	}

	//UTILS
	bool inRange(int num, int min, int max) {
		return (num >= min && num <= max);
	}

	bool hovering(SDL_FRect rect) {
		return inRange(Mouse.x, rect.x, rect.x + rect.w) && inRange(Mouse.y, rect.y, rect.y + rect.h);
	}

	void fullscreen(bool yes) {
		SDL_SetWindowFullscreen(win, yes ? SDL_WINDOW_FULLSCREEN : 0);
	}

	void setTimeout(std::function<void()> function, int ms) {
		std::thread([function, ms]() {
			SDL_Delay(ms);
			function();
		}).detach();
	}

} // namespace AST


namespace SpriteManager {
#ifdef AST_TEXTURE
	std::vector<std::pair<SDL_Texture*, std::string>> sprites;

	bool load(std::string keyword, std::string file) {
	    for (auto sprite : sprites)
	        if (sprite.second == keyword && sprite.first != nullptr)
	            return true;

	    SDL_Texture* texture = IMG_LoadTexture(AST::ren, ("Resources/GFX" + file).c_str());
	    if (texture == nullptr)
	        return false;

	    sprites.push_back({ texture, keyword });
	    return true;
	}

	bool load(std::string keyword, std::string sheetFile, SDL_Rect spriteRect) {
	    for (auto sprite : sprites) {
	        if (sprite.second == keyword && sprite.first != nullptr)
	            return true;
	    }

	    SDL_Texture* sheetTexture = IMG_LoadTexture(AST::ren, ("Resources/GFX" + sheetFile).c_str());
	    if (sheetTexture == nullptr)
	        return false;

	    SDL_Texture* spriteTexture = SDL_CreateTexture(AST::ren, SDL_PIXELFORMAT_UNKNOWN, SDL_TEXTUREACCESS_TARGET, spriteRect.w, spriteRect.h);
	    if (spriteTexture == nullptr) {
	        SDL_DestroyTexture(sheetTexture);
	        return false;
	    }

	    SDL_SetRenderTarget(AST::ren, spriteTexture);
	    SDL_RenderCopy(AST::ren, sheetTexture, &spriteRect, nullptr);
	    SDL_SetRenderTarget(AST::ren, nullptr);
	    sprites.push_back({ spriteTexture, keyword });

	    SDL_DestroyTexture(sheetTexture);

	    return true;
	}

	bool drawTRect(AST::Rect rect) {
		bool found = false;

		for (auto sprite : sprites) {
			if (sprite.second == rect.texture) {
				#ifdef AST_PHYSICS
					SDL_RenderCopyF(AST::ren, sprite.first, nullptr, &rect);
				#else
					SDL_RenderCopyExF(AST::ren, sprite.first, nullptr, &rect, rect.angle, nullptr, SDL_FLIP_NONE);
				#endif
				found = true;
				break;
			}
		}

		return found;
	}

	void free() {
		for (auto& sprite : sprites) SDL_DestroyTexture(sprite.first);
		sprites.clear();
	}

#endif
	void drawCRect(AST::Rect rect) {
		SDL_SetRenderDrawColor(AST::ren, rect.start.r, rect.start.g, rect.start.b, rect.start.a);
		SDL_RenderFillRectF(AST::ren, &rect);
		SDL_SetRenderDrawColor(AST::ren, 0, 0, 0, 255);
	}

	void drawGRect(AST::Rect rect) {
		// Calculate the color step for each pixel
		float rStep = (float)(rect.end.r - rect.start.r) / rect.h;
		float gStep = (float)(rect.end.g - rect.start.g) / rect.h;
		float bStep = (float)(rect.end.b - rect.start.b) / rect.h;

		// Loop through each row of the rectangle
		for (int y = rect.y; y < rect.y + rect.h; ++y) {
			// Calculate the color for the current row
			Uint8 r = rect.start.r + (rStep * (y - rect.y));
			Uint8 g = rect.start.g + (gStep * (y - rect.y));
			Uint8 b = rect.start.b + (bStep * (y - rect.y));

			// Set the color for the current row
			SDL_SetRenderDrawColor(AST::ren, r, g, b, 255);

			// Draw a horizontal line for the current row
			SDL_RenderDrawLine(AST::ren, rect.x, y, rect.x + rect.w, y);
		}
		SDL_SetRenderDrawColor(AST::ren, 0, 0, 0, 255);
	}

	bool draw(AST::Rect rect) {
		switch(rect.type) {
			#ifdef AST_TEXTURE
			case 0: {
				return drawTRect(rect);
			} 
			#endif
			case 1: {
				drawCRect(rect);
				return true;
			}
			case 2: {
				drawGRect(rect);
				return true;
			}
			default: {
				return false;
			}
		}
	}

} // namespace SpriteManager