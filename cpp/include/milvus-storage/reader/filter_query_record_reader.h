#pragma once
#include <arrow/filesystem/filesystem.h>
#include <arrow/record_batch.h>
#include <parquet/arrow/reader.h>
#include "file/delete_fragment.h"
#include "file/fragment.h"
#include "format/parquet/file_reader.h"
#include "reader/multi_files_sequential_reader.h"
#include "storage/space.h"
namespace milvus_storage {

class FilterQueryRecordReader : public arrow::RecordBatchReader {
  public:
  FilterQueryRecordReader(std::shared_ptr<ReadOptions> options,
                          const FragmentVector& scalar_fragments,
                          const FragmentVector& vector_fragments,
                          const DeleteFragmentVector& delete_fragments,
                          std::shared_ptr<arrow::fs::FileSystem> fs,
                          std::shared_ptr<Schema> schema);

  std::shared_ptr<arrow::Schema> schema() const override;

  arrow::Status ReadNext(std::shared_ptr<arrow::RecordBatch>* batch) override;

  private:
  // Try to make inner reader, return nullptr if next_pos_ reach the end.
  Result<std::shared_ptr<arrow::RecordBatchReader>> MakeInnerReader();

  std::shared_ptr<arrow::fs::FileSystem> fs_;
  std::shared_ptr<Schema> schema_;
  std::shared_ptr<ReadOptions> options_;
  DeleteFragmentVector delete_fragments_;

  std::vector<std::string> scalar_files_;
  std::vector<std::string> vector_files_;
  int64_t next_pos_ = 0;

  std::shared_ptr<parquet::arrow::FileReader> holding_scalar_file_reader_;
  std::shared_ptr<parquet::arrow::FileReader> holding_vector_file_reader_;

  std::shared_ptr<arrow::RecordBatchReader> curr_reader_;
};
}  // namespace milvus_storage
