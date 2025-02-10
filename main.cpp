#include <QApplication>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QTimer>
#include <cmath>
#include <algorithm> // Для std::min
#include <iostream>

const float PI = 3.14159265f;

struct Segment {
    QPointF start;
    float length;
    float angle; // угол в радианах
    QGraphicsLineItem *bone;

    Segment(float x, float y, float length, float angle, QGraphicsScene* scene)
        : start(x, y), length(length), angle(angle) {
        bone = new QGraphicsLineItem();
        bone->setPen(QPen(Qt::black, 5));
        update(QPointF(x, y), angle);
        scene->addItem(bone);
    }

    void update(QPointF newStart, float newAngle) {
        start = newStart;
        angle = newAngle;
        QPointF end = getEnd();
        bone->setLine(start.x(), start.y(), end.x(), end.y());
    }

    QPointF getEnd() const {
        float endX = start.x() + length * std::cos(angle);
        float endY = start.y() + length * std::sin(angle);
        return QPointF(endX, endY);
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QGraphicsScene scene;
    QGraphicsView view(&scene);
    view.setRenderHint(QPainter::Antialiasing);
    view.setFixedSize(800, 600);
    view.setSceneRect(-400, -300, 800, 600);

    // Торс (базовая точка)
    Segment torso(0, 0, 100, -PI / 2, &scene);

    // Ноги
    //Segment leftThigh(torso.getEnd().x(), torso.getEnd().y(), 60, PI / 2, &scene);
    //Segment leftShin(leftThigh.getEnd().x(), leftThigh.getEnd().y(), 60, PI / 2, &scene);

    //Segment rightThigh(torso.getEnd().x(), torso.getEnd().y(), 60, PI / 2, &scene);
    //Segment rightShin(rightThigh.getEnd().x(), rightThigh.getEnd().y(), 60, PI / 2, &scene);
    Segment leftThigh(torso.start.x(), torso.start.y(), 60, PI / 2, &scene);
    Segment leftShin(leftThigh.getEnd().x(), leftThigh.getEnd().y(), 60, PI / 2, &scene);

    Segment rightThigh(torso.start.x(), torso.getEnd().y(), 60, PI / 2, &scene);
    Segment rightShin(rightThigh.getEnd().x(), rightThigh.getEnd().y(), 60, PI / 2, &scene);

    // Руки
    Segment leftUpperArm(torso.start.x(), torso.start.y(), 50, PI / 2, &scene);
    Segment leftForearm(leftUpperArm.getEnd().x(), leftUpperArm.getEnd().y(), 50, PI / 2, &scene);

    Segment rightUpperArm(torso.start.x(), torso.start.y(), 50, PI / 2, &scene);
    Segment rightForearm(rightUpperArm.getEnd().x(), rightUpperArm.getEnd().y(), 50, PI / 2, &scene);

    // Голова
    QGraphicsEllipseItem *head = new QGraphicsEllipseItem(-20, -20, 40, 40);
    head->setBrush(Qt::black);
    scene.addItem(head);

    float time = 0.0f;
    float walkSpeed = 4.0f;

    QTimer timer;
    QObject::connect(&timer, &QTimer::timeout, [&]() {
        time += 0.01f;

        // Амплитуды колебаний
        float thighAmplitude = PI / 8;   // Амплитуда колебания для бедра
        float shinAmplitude = PI / 10;  // Амплитуда колебания для голени
        float armAmplitude = PI / 10;   // Амплитуда колебания для плеча
        float forearmAmplitude = PI / 12; // Амплитуда колебания для предплечья

        // Колебания ног
        leftThigh.angle = PI/2 + std::sin(time * walkSpeed) * thighAmplitude;
        leftShin.angle = std::min(PI / 2 + std::sin(time * walkSpeed + PI / 4) * shinAmplitude, leftThigh.angle);

        rightThigh.angle = PI/2 +  std::sin(time * walkSpeed + PI) * thighAmplitude;
        rightShin.angle = std::min(PI / 2 + std::sin(time * walkSpeed + PI + PI / 4) * shinAmplitude, rightThigh.angle);

        // Колебания рук
        leftUpperArm.angle = PI/2 +  std::sin(time * walkSpeed + PI) * armAmplitude;
        leftForearm.angle = std::max(PI / 2 + std::sin(time * walkSpeed + PI + PI / 6) * forearmAmplitude, leftUpperArm.angle);
        std::cout << leftUpperArm.angle << " " << leftForearm.angle << std::endl;

        rightUpperArm.angle = PI/2 +  std::sin(time * walkSpeed) * armAmplitude;
        rightForearm.angle = std::max(PI / 2 + std::sin(time * walkSpeed + PI / 6) * forearmAmplitude, rightUpperArm.angle);

        // Обновляем сегменты с помощью прямой кинематики
        torso.update(torso.start, -PI / 2);

        leftThigh.update(torso.getEnd(), leftThigh.angle);
        leftShin.update(leftThigh.getEnd(), leftShin.angle);

        rightThigh.update(torso.getEnd(), rightThigh.angle);
        rightShin.update(rightThigh.getEnd(), rightShin.angle);

        leftUpperArm.update(torso.start, leftUpperArm.angle);
        leftForearm.update(leftUpperArm.getEnd(), leftForearm.angle);

        rightUpperArm.update(torso.start, rightUpperArm.angle);
        rightForearm.update(rightUpperArm.getEnd(), rightForearm.angle);

        // Голова движется вместе с торсом
        head->setPos(torso.start.x(), torso.start.y() - 125);
    });

    timer.start(16); // Обновление каждые 16 мс (примерно 60 FPS)

    view.show();
    return app.exec();
}
