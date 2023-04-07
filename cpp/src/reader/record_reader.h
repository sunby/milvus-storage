#pragma once

#include <memory>

#include "reader/scan_record_reader.h"
#include "reader/merge_record_reader.h"
#include "reader/filter_query_record_reader.h"
#include "storage/default_space.h"
namespace milvus_storage {

struct RecordReader {
  static std::unique_ptr<arrow::RecordBatchReader>
  GetRecordReader(const DefaultSpace& space, std::shared_ptr<ReadOptions>& options) {
    std::set<std::string> related_columns;
    for (auto& column : options->columns) {
      related_columns.insert(column);
    }
    for (auto& filter : options->filters) {
      related_columns.insert(filter->get_column_name());
    }

    if (only_contain_scalar_columns(space, related_columns)) {
      return std::unique_ptr<arrow::RecordBatchReader>(
          new ScanRecordReader(options, space.manifest_->scalar_files(), space));
    }

    if (only_contain_vector_columns(space, related_columns)) {
      return std::unique_ptr<arrow::RecordBatchReader>(
          new ScanRecordReader(options, space.manifest_->vector_files(), space));
    }

    if (filters_only_contain_pk_and_version(space, options->filters)) {
      return std::unique_ptr<arrow::RecordBatchReader>(
          new MergeRecordReader(options, space.manifest_->scalar_files(), space.manifest_->vector_files(), space));
    } else {
      return std::unique_ptr<arrow::RecordBatchReader>(new FilterQueryRecordReader(
          options, space.manifest_->scalar_files(), space.manifest_->vector_files(), space));
    }
  }

  static bool
  only_contain_scalar_columns(const DefaultSpace& space, const std::set<std::string>& related_columns) {
    for (auto& column : related_columns) {
      if (space.schema_->options()->vector_column == column) {
        return false;
      }
    }
    return true;
  }

  static bool
  only_contain_vector_columns(const DefaultSpace& space, const std::set<std::string>& related_columns) {
    for (auto& column : related_columns) {
      if (space.schema_->options()->vector_column != column && space.schema_->options()->primary_column != column &&
          space.schema_->options()->version_column != column) {
        return false;
      }
    }
    return true;
  }

  static bool
  filters_only_contain_pk_and_version(const DefaultSpace& space, const std::vector<Filter*>& filters) {
    for (auto& filter : filters) {
      if (filter->get_column_name() != space.schema_->options()->primary_column &&
          filter->get_column_name() != space.schema_->options()->version_column) {
        return false;
      }
    }
    return true;
  }
};
}  // namespace milvus_storage