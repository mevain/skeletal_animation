#include <QApplication>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QTimer>
#include <cmath>

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

    // Основное тело
    Segment torso(-200, 0, 100, -PI / 2, &scene);

    // Левая и правая ноги, установленные вертикально
    Segment leftThigh(torso.getEnd().x(), torso.getEnd().y(), 60, PI / 2, &scene);
    Segment leftShin(leftThigh.getEnd().x(), leftThigh.getEnd().y(), 60, PI / 2, &scene);

    Segment rightThigh(torso.getEnd().x(), torso.getEnd().y(), 60, PI / 2, &scene);
    Segment rightShin(rightThigh.getEnd().x(), rightThigh.getEnd().y(), 60, PI / 2, &scene);

    // Левая и правая руки, установленные вертикально
    Segment leftUpperArm(torso.start.x(), torso.start.y(), 50, PI / 2, &scene);
    Segment leftForearm(leftUpperArm.getEnd().x(), leftUpperArm.getEnd().y(), 50, PI / 2, &scene);

    Segment rightUpperArm(torso.start.x(), torso.start.y(), 50, PI / 2, &scene);
    Segment rightForearm(rightUpperArm.getEnd().x(), rightUpperArm.getEnd().y(), 50, PI / 2, &scene);

    // Голова
    QGraphicsEllipseItem head;
    head.setRect(-20, -20, 40, 40);
    head.setBrush(Qt::black);
    scene.addItem(&head);

    float time = 0.0f;
    float walkSpeed = 2.0f;
    float torsoSpeed = 1.0f;

    QTimer timer;
    QObject::connect(&timer, &QTimer::timeout, [&]() {
        time += 0.01f;

        // Колебания ног вокруг вертикальной оси
        leftThigh.angle = PI / 2 + std::sin(time * walkSpeed) * PI / 8;
        leftShin.angle = PI / 2 + std::sin(time * walkSpeed + PI / 4) * PI / 10;

        rightThigh.angle = PI / 2 + std::sin(time * walkSpeed + PI) * PI / 8;
        rightShin.angle = PI / 2 + std::sin(time * walkSpeed + PI + PI / 4) * PI / 10;

        // Колебания рук вокруг вертикальной оси в противофазе с ногами
        leftUpperArm.angle = PI / 2 + std::sin(time * walkSpeed + PI) * PI / 10;
        leftForearm.angle = PI / 2 + std::sin(time * walkSpeed + PI + PI / 6) * PI / 12;

        rightUpperArm.angle = PI / 2 + std::sin(time * walkSpeed) * PI / 10;
        rightForearm.angle = PI / 2 + std::sin(time * walkSpeed + PI / 6) * PI / 12;

        // Смещаем torso по оси X и добавляем небольшие колебания по оси Y
        torso.start.setX(torso.start.x() + torsoSpeed);
        //torso.start.setY(10 * std::sin(time * walkSpeed));  // колебание по вертикали

        // Обновляем позиции суставов и отображение torso
        torso.update(torso.start, -PI / 2);  // Обновляем отображение torso

        leftThigh.update(torso.getEnd(), leftThigh.angle);
        leftShin.update(leftThigh.getEnd(), leftShin.angle);

        rightThigh.update(torso.getEnd(), rightThigh.angle);
        rightShin.update(rightThigh.getEnd(), rightShin.angle);

        leftUpperArm.update(torso.start, leftUpperArm.angle);
        leftForearm.update(leftUpperArm.getEnd(), leftForearm.angle);

        rightUpperArm.update(torso.start, rightUpperArm.angle);
        rightForearm.update(rightUpperArm.getEnd(), rightForearm.angle);

        // Обновляем позицию головы вместе с торсом
        head.setPos(torso.start.x(), torso.start.y()-125);  // голова сдвинута выше торса
    });

    timer.start(16); // примерно 60 кадров в секунду

    view.show();
    return app.exec();
}
