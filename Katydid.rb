require "formula"

class Katydid < Formula
  homepage "https://github.com/project8/katydid"
  head "https://github.com/project8/katydid.git", :tag => "v2.7.2"
  sha256 "a6b865fe9a1e8fd223af1fd7164e66c1451c461b580b723ad2ee685b825a22b4"
  version "2.7.2"

  depends_on "cmake"  => :build
  depends_on "Boost"  => :build
  depends_on "hdf5"  => :build
  depends_on "protobuf"  => :build
  depends_on "fftw"  => :build
  depends_on "root"  => :build
  depends_on "libmatio"  => :build
  depends_on "protobuf"  => :build
  depends_on "python" => :optional
  # TODO: Add debug flag

  def install
    system "echo #{prefix}"
    mkdir "builddir" do
      system "cmake", "..", *std_cmake_args
      system "make", "-j3"
      system "pwd"
      system "make", "-j3", "install", "DESTDIR=#{prefix}"
      system "mv", "/usr/local/Cellar/katydid/*/tmp/*/builddir", "#{prefix}" 

      bin.install Dir["bin/*"]
      # chmod 0755, Dir[bin/"*.*sh"]
    end
  end

  # TODO: bottle do ...

  test do
    system "#{bin}/katydid", "--help"
  end
end
