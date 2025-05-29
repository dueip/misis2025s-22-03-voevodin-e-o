#include <opencv2/opencv.hpp>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>
#include <map>
#include <iomanip>
#include <filesystem>

struct Ellipse {
    double center_x;  // ��������� ���������� � ������
    double center_y;
    double width;
    double height;
    double angle;
    int row;          // ������� � �������
    int col;
};

struct Metrics {
    int TP = 0;       // True positives
    int FP = 0;       // False positives
    int FN = 0;       // False negatives
    double precision = 0.0;
    double recall = 0.0;
    double f1 = 0.0;
};

// ������� ��� ������� IoU ���� ��������
double calculateIOU(const Ellipse& ref, const Ellipse& det) {
    // ������� ����������� ��� �����
    const int cell_size = 256;
    cv::Mat mask_ref = cv::Mat::zeros(cell_size, cell_size, CV_8UC1);
    cv::Mat mask_det = cv::Mat::zeros(cell_size, cell_size, CV_8UC1);

    // ������� RotatedRect � ��������� �����������
    cv::RotatedRect rr_ref(
        cv::Point2f(ref.center_x, ref.center_y),
        cv::Size2f(ref.width, ref.height),
        ref.angle
    );

    cv::RotatedRect rr_det(
        cv::Point2f(det.center_x, det.center_y),
        cv::Size2f(det.width, det.height),
        det.angle
    );

    // ������ �������
    cv::ellipse(mask_ref, rr_ref, cv::Scalar(255), -1);
    cv::ellipse(mask_det, rr_det, cv::Scalar(255), -1);

    // ������������ ����������� � �����������
    cv::Mat intersection, union_mat;
    cv::bitwise_and(mask_ref, mask_det, intersection);
    cv::bitwise_or(mask_ref, mask_det, union_mat);

    double intersection_area = cv::countNonZero(intersection);
    double union_area = cv::countNonZero(union_mat);

    return (union_area > 0) ? (intersection_area / union_area) : 0.0;
}

// �������� ������ ������
std::vector<std::string> loadFileList(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file list: " + filePath);
    }

    std::vector<std::string> files;
    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty()) {
            files.push_back(line);
        }
    }
    return files;
}

// �������� ��������� ������ (�� ����������)
std::vector<Ellipse> loadReferenceEllipses(const std::string& filePath) {
    std::ifstream in(filePath);
    if (!in.is_open()) {
        throw std::runtime_error("Could not open reference file: " + filePath);
    }

    std::vector<Ellipse> ellipses;

    // ���������� ���������� ��������� (9 ��������)
    double dummy;
    for (int i = 0; i < 9; i++) {
        in >> dummy;
    }

    int count;
    in >> count;

    for (int i = 0; i < count; i++) {
        Ellipse e;
        in >> e.width;       // ������
        in >> e.height;      // ������
        in >> e.angle;       // ����
        in >> e.center_x;    // ����� X (���������)
        in >> e.center_y;    // ����� Y (���������)
        in >> e.row;         // ������
        in >> e.col;         // �������
        ellipses.push_back(e);
    }
    return ellipses;
}

// �������� ����������� �������������� (�� ���������)
std::vector<Ellipse> loadDetectedEllipses(const std::string& filePath) {
    std::ifstream in(filePath);
    if (!in.is_open()) {
        throw std::runtime_error("Could not open detection file: " + filePath);
    }

    std::vector<Ellipse> ellipses;
    int count;
    in >> count;

    for (int i = 0; i < count; i++) {
        Ellipse e;
        in >> e.center_x;    // ����� X (���������)
        in >> e.center_y;    // ����� Y (���������)
        in >> e.width;       // ������
        in >> e.height;      // ������
        in >> e.angle;       // ����
        in >> e.row;         // ������
        in >> e.col;         // �������
        ellipses.push_back(e);
    }
    return ellipses;
}

// ������ ������ ��� ������ �����������
Metrics calculateMetricsForImage(
    const std::vector<Ellipse>& references,
    const std::vector<Ellipse>& detections,
    double iou_threshold = 0.5
) {
    Metrics metrics;
    std::vector<bool> matched_detections(detections.size(), false);
    std::vector<bool> matched_references(references.size(), false);

    // �������� ��������� ���� ��������
    for (size_t i = 0; i < references.size(); i++) {
        for (size_t j = 0; j < detections.size(); j++) {
            if (matched_detections[j]) continue;

            // ���������� ������ ������� � ����� ������
            if (references[i].row != detections[j].row ||
                references[i].col != detections[j].col) {
                continue;
            }

            double iou = calculateIOU(references[i], detections[j]);
            if (iou >= iou_threshold) {
                metrics.TP++;
                matched_detections[j] = true;
                matched_references[i] = true;
                break;
            }
        }
    }

    // ���������������� �������� - FP
    for (bool matched : matched_detections) {
        if (!matched) metrics.FP++;
    }

    // ���������������� ������� - FN
    for (bool matched : matched_references) {
        if (!matched) metrics.FN++;
    }

    // ������� precision � recall
    metrics.precision = (metrics.TP + metrics.FP) > 0 ?
        static_cast<double>(metrics.TP) / (metrics.TP + metrics.FP) : 0.0;

    metrics.recall = (metrics.TP + metrics.FN) > 0 ?
        static_cast<double>(metrics.TP) / (metrics.TP + metrics.FN) : 0.0;

    // ������� F1-score
    metrics.f1 = (metrics.precision + metrics.recall) > 0 ?
        2 * metrics.precision * metrics.recall / (metrics.precision + metrics.recall) : 0.0;

    return metrics;
}

// ��������� ������
void generateReport(
    const std::string& protocolPath,
    const std::vector<std::string>& refFiles,
    const std::vector<std::string>& detFiles
) {
    if (refFiles.size() != detFiles.size()) {
        throw std::runtime_error("Reference and detection file lists must have the same size");
    }

    std::ofstream report(protocolPath);
    if (!report.is_open()) {
        throw std::runtime_error("Could not open protocol file: " + protocolPath);
    }

    // ��������� ������
    report << "Quality Assessment Report\n";
    report << "=========================\n\n";
    report << "Files processed: " << refFiles.size() << "\n\n";

    report << "| File Name          | TP | FP | FN | Precision | Recall   | F1-score |\n";
    report << "|--------------------|----|----|----|-----------|----------|----------|\n";

    Metrics total_metrics;

    // ������������ ������ ���� ������
    for (size_t i = 0; i < refFiles.size(); i++) {
        try {
            auto references = loadReferenceEllipses(refFiles[i]);
            auto detections = loadDetectedEllipses(detFiles[i]);

            Metrics img_metrics = calculateMetricsForImage(references, detections);

            // ��������� � ����� ��������
            total_metrics.TP += img_metrics.TP;
            total_metrics.FP += img_metrics.FP;
            total_metrics.FN += img_metrics.FN;

            // ����������� ����� ��� �������� �����
            report << "| " << std::setw(18) << std::left << detFiles[i] << " | "
                << std::setw(2) << img_metrics.TP << " | "
                << std::setw(2) << img_metrics.FP << " | "
                << std::setw(2) << img_metrics.FN << " | "
                << std::fixed << std::setprecision(4) << std::setw(9) << img_metrics.precision << " | "
                << std::setw(8) << img_metrics.recall << " | "
                << std::setw(8) << img_metrics.f1 << " |\n";

        }
        catch (const std::exception& e) {
            std::cerr << "Error processing image pair " << i << ": " << e.what() << std::endl;
            report << "| " << std::setw(18) << std::left << detFiles[i] << " | " << "ERROR" << std::string(55, ' ') << " |\n";
        }
    }

    // ������� ����� ������
    double precision = (total_metrics.TP + total_metrics.FP) > 0 ?
        static_cast<double>(total_metrics.TP) / (total_metrics.TP + total_metrics.FP) : 0.0;

    double recall = (total_metrics.TP + total_metrics.FN) > 0 ?
        static_cast<double>(total_metrics.TP) / (total_metrics.TP + total_metrics.FN) : 0.0;

    double f1 = (precision + recall) > 0 ?
        2 * precision * recall / (precision + recall) : 0.0;

    // �������� ������
    report << "\nSummary:\n";
    report << "Total True Positives (TP): " << total_metrics.TP << "\n";
    report << "Total False Positives (FP): " << total_metrics.FP << "\n";
    report << "Total False Negatives (FN): " << total_metrics.FN << "\n";
    report << std::fixed << std::setprecision(4);
    report << "Precision: " << precision << "\n";
    report << "Recall: " << recall << "\n";
    report << "F1-score: " << f1 << "\n";
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0]
            << " <ref_list> <det_list> <protocol_file>\n";
        return 1;
    }

    std::string refListPath = argv[1];
    std::string detListPath = argv[2];
    std::string protocolPath = argv[3];

    try {
        // �������� ������� ������
        auto refFiles = loadFileList(refListPath);
        auto detFiles = loadFileList(detListPath);

        // ��������� ������
        generateReport(protocolPath, refFiles, detFiles);

        std::cout << "Report successfully generated: " << protocolPath << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}